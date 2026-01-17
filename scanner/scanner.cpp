#include "scanner.h"
#include "hashDatabase.h"
#include "logger.h"
#include "md5Calc.h"
#include "threadPool.h"
#include "utils.h"
#include "settingsValidator.h"
#include "scannerConstants.h"

#include <stdexcept>
#include <iostream>

namespace Scanner {

ScannerImpl::ScannerImpl() 
    : isScanning_(false), stopRequested_(false),
      totalFiles_(0), malwareFiles_(0), errors_(0) {
}

ScannerImpl::~ScannerImpl() {
    Stop();
}

ScanResult ScannerImpl::Scan(const ScanSettings& settings) {
    return ScanWithProgress(settings, nullptr);
}

ScanResult ScannerImpl::ScanWithProgress(const ScanSettings& settings, ProgressCallback callback) {
    if (isScanning_) {
        throw std::runtime_error("Scan already in progress");
    }
    
    // Validate settings before starting
    if (auto error = SettingsValidator::Validate(settings)) {
        throw std::runtime_error("Invalid scan settings: " + *error);
    }
    
    isScanning_ = true;
    stopRequested_ = false;
    progressCallback_ = callback;
    
    totalFiles_ = 0;
    malwareFiles_ = 0;
    errors_ = 0;
    detectedMalware_.clear();
    
    startTime_ = std::chrono::steady_clock::now();
    
    try {
        InitializeDependencies(settings);
        ExecuteScan(settings);
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->LogError(std::string("Fatal error: ") + e.what());
        }
        errors_++;
        isScanning_ = false;
        throw;  // Re-throw to caller
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    
    ScanResult result;
    result.totalFilesProcessed = totalFiles_;
    result.malwareFilesDetected = malwareFiles_;
    result.errorsCount = errors_;
    result.executionTime = duration;
    result.detectedMalware = detectedMalware_;
    
    isScanning_ = false;
    
    return result;
}

void ScannerImpl::InitializeDependencies(const ScanSettings& settings) {
    // Create logger using factory method
    logger_ = Logger::Create(settings.logPath);
    logger_->LogInfo("Initializing malware scanner");
    
    // Load malware database
    database_ = std::make_unique<HashDatabase>();
    if (!database_->LoadFromCSV(settings.databasePath)) {
        throw std::runtime_error("Failed to load hash database from: " + settings.databasePath);
    }
    logger_->LogInfo("Loaded " + std::to_string(database_->GetSize()) + " malware signatures");
    
    // Initialize thread pool
    size_t threadCount = settings.threadCount;
    if (threadCount == 0) {
        threadCount = Utils::GetHardwareConcurrency();
    }
    threadPool_ = std::make_unique<ThreadPool>(threadCount);
    logger_->LogInfo("Using " + std::to_string(threadCount) + " threads");
}

void ScannerImpl::ExecuteScan(const ScanSettings& settings) {
    std::vector<std::filesystem::path> files;
    CollectFiles(settings.rootPath, files);
    
    logger_->LogInfo("Found " + std::to_string(files.size()) + " files to scan");
    
    for (const auto& file : files) {
        if (stopRequested_) {
            logger_->LogInfo("Scan stopped by user");
            break;
        }
        
        threadPool_->Enqueue([this, file]() {
            if (!stopRequested_) {
                ProcessFile(file);
            }
        });
    }
    
    // Wait for all tasks to complete
    threadPool_->Wait();
    logger_->LogInfo("Scan completed");
}

void ScannerImpl::Stop() {
    stopRequested_ = true;
    if (threadPool_) {
        threadPool_->Stop();
    }
}

bool ScannerImpl::IsScanning() const {
    return isScanning_;
}

void ScannerImpl::CollectFiles(const std::filesystem::path& root, 
                               std::vector<std::filesystem::path>& files) {
    try {
        std::error_code ec;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(
                root, 
                std::filesystem::directory_options::skip_permission_denied,
                ec)) {
            
            if (stopRequested_) {
                break;
            }
            
            if (ec) {
                if (logger_) {
                    logger_->LogError("Error iterating directory: " + ec.message());
                }
                errors_++;
                ec.clear();
                continue;
            }
            
            if (entry.is_regular_file(ec)) {
                // Check file size before adding
                auto fileSize = entry.file_size(ec);
                if (ec) {
                    if (logger_) {
                        logger_->LogError("Cannot get file size: " + entry.path().string());
                    }
                    errors_++;
                    ec.clear();
                    continue;
                }
                
                if (fileSize > Constants::MAX_FILE_SIZE) {
                    if (logger_) {
                        logger_->LogError("File too large, skipping: " + entry.path().string() + 
                                         " (" + std::to_string(fileSize) + " bytes)");
                    }
                    errors_++;
                    continue;
                }
                
                files.push_back(entry.path());
            }
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->LogError("Error collecting files: " + std::string(e.what()));
        }
        errors_++;
    }
}

void ScannerImpl::ProcessFile(const std::filesystem::path& filepath) {
    totalFiles_++;
    
    if (progressCallback_) {
        std::lock_guard<std::mutex> lock(progressMutex_);
        progressCallback_(filepath.string(), totalFiles_);
    }
    
    try {
        if (!Utils::IsFileReadable(filepath)) {
            logger_->LogError("Cannot read file: " + filepath.string());
            errors_++;
            return;
        }
        
        std::string hash = MD5Calculator::CalculateFile(filepath);
        std::string verdict;

        if (database_->IsMalicious(hash, verdict)) {
            MalwareInfo info;
            info.filePath = filepath.string();
            info.hash = hash;
            info.verdict = verdict;
            logger_->LogMalware(info);
            
            {
                std::lock_guard<std::mutex> lock(resultMutex_);
                detectedMalware_.push_back(info);
            }
            
            malwareFiles_++;
        }
        
    } catch (const std::exception& e) {
        logger_->LogError("Error processing file " + filepath.string() + ": " + e.what());
        errors_++;
    }
}

} // namespace Scanner


extern "C" SCANNER_API Scanner::IScanner* CreateScanner() {
    return new Scanner::ScannerImpl();
}

extern "C" SCANNER_API void DestroyScanner(Scanner::IScanner* scanner) {
    delete scanner;
}