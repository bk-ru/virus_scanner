#include "scanner.h"
#include "hashDatabase.h"
#include "logger.h"
#include "md5Calc.h"
#include "threadPool.h"
#include "utils.h"

#include <stdexcept>
#include <iostream>
#include <openssl/md5.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

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
    
    isScanning_ = true;
    stopRequested_ = false;
    progressCallback_ = callback;
    
    // Сброс счетчиков
    totalFiles_ = 0;
    malwareFiles_ = 0;
    errors_ = 0;
    detectedMalware_.clear();
    
    startTime_ = std::chrono::steady_clock::now();
    
    try {
        logger_ = std::make_unique<Logger>(settings.logPath);
        
        // Загрузка базы данных хешей
        database_ = std::make_unique<HashDatabase>();
        if (!database_->LoadFromCSV(settings.databasePath)) {
            throw std::runtime_error("Failed to load hash database from: " + settings.databasePath);
        }
        
        // Создание пула потоков
        size_t threadCount = settings.threadCount;
        if (threadCount == 0) {
            threadCount = Utils::GetHardwareConcurrency();
        }
        threadPool_ = std::make_unique<ThreadPool>(threadCount);
        
        // Сбор файлов для сканирования
        std::vector<std::filesystem::path> files;
        CollectFiles(settings.rootPath, files);
        
        // Обработка файлов
        for (const auto& file : files) {
            if (stopRequested_) break;
            
            threadPool_->Enqueue([this, file]() {
                if (!stopRequested_) {
                    ProcessFile(file);
                }
            });
        }
        
        // Ожидание завершения всех задач
        threadPool_->Wait();
        
    } catch (const std::exception& e) {
        logger_->LogError(std::string("Fatal error: ") + e.what());
        errors_++;
    }
    
    // Формирование результата
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
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (stopRequested_) break;
            
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }
    } catch (const std::exception& e) {
        logger_->LogError("Error collecting files: " + std::string(e.what()));
        errors_++;
    }
}

void ScannerImpl::ProcessFile(const std::filesystem::path& filepath) {
    totalFiles_++;
    
    // Вызов callback прогресса
    if (progressCallback_) {
        std::lock_guard<std::mutex> lock(progressMutex_);
        progressCallback_(filepath.string(), totalFiles_);
    }
    
    try {
        // Проверка доступа к файлу
        if (!Utils::IsFileReadable(filepath)) {
            logger_->LogError("Cannot read file: " + filepath.string());
            errors_++;
            return;
        }
        
        // Вычисление MD5
        std::string hash = MD5Calculator::CalculateFile(filepath);
        
        // Проверка в базе данных
        std::string verdict;
        if (database_->IsMalicious(hash, verdict)) {
            MalwareInfo info;
            info.filePath = filepath.string();
            info.hash = hash;
            info.verdict = verdict;
            
            // Логирование
            logger_->LogMalware(info);
            
            // Сохранение результата
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

// Экспортируемые функции
extern "C" SCANNER_API Scanner::IScanner* CreateScanner() {
    std::cerr << "CreateScanner() called\n";
    return new Scanner::ScannerImpl();
}

const char* GetLibraryVersion() {
    return "1.0.0";
}

extern "C" SCANNER_API void DestroyScanner(Scanner::IScanner* scanner) {
    delete scanner;
}