#include "config.h"
#include "lineParser.h"
#include "scannerApi.h"

#include <iostream>
#include <chrono>
#include <memory>
#include <thread>

int main(int argc, char* argv[])
{
    try {
        console::Config config(false);
        console::LineParser parser(config);

        if (!parser.parse(argc, argv))
            return 1;

        std::unique_ptr<Scanner::IScanner> scanner(CreateScanner());
        if (!scanner) {
            std::cerr << "Failed to create scanner instance" << std::endl;
            return 1;
        }

        Scanner::ScanSettings settings;
        settings.rootPath = config.GetScanPath();
        settings.databasePath = config.GetHashDatabasePath();
        settings.logPath = config.GetLogPath();
        settings.threadCount = std::thread::hardware_concurrency();

        std::cout << "Starting malware scan..." << std::endl;
        std::cout << "Root path: " << settings.rootPath << std::endl;
        std::cout << "Database: " << settings.databasePath << std::endl;
        std::cout << "Log file: " << settings.logPath << std::endl;
        std::cout << "Thread count: " << settings.threadCount << std::endl;
        std::cout << std::endl;

        // auto progressCallback = [](const std::string& currentFile, size_t processedFiles) {
        //     std::cout << "\rProcessed files: " << processedFiles 
        //               << " | Current: " << currentFile << std::flush;
        // };

        auto start = std::chrono::steady_clock::now();
        // Scanner::ScanResult result = scanner->ScanWithProgress(settings, progressCallback);
        Scanner::ScanResult result = scanner->Scan(settings);
        auto end = std::chrono::steady_clock::now();

        std::cout << "=== SCAN REPORT ===" << std::endl;
        std::cout << "Total files processed: " << result.totalFilesProcessed << std::endl;
        std::cout << "Malware files detected: " << result.malwareFilesDetected << std::endl;
        std::cout << "Errors during analysis: " << result.errorsCount << std::endl;
        std::cout << "Execution time: " << result.executionTime.count() << " ms" << std::endl;

        // if (result.malwareFilesDetected > 0) {
        //     std::cout << std::endl << "=== DETECTED MALWARE ===" << std::endl;
        //     for (const auto& malware : result.detectedMalware) {
        //         std::cout << "File: " << malware.filePath << std::endl;
        //         std::cout << "Hash: " << malware.hash << std::endl;
        //         std::cout << "Verdict: " << malware.verdict << std::endl;
        //         std::cout << "---" << std::endl;
        //     }
        // }

        DestroyScanner(scanner.release());
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}