#pragma once

#ifdef _WIN32
    #ifdef SCANNER_DLL_EXPORTS
        #define SCANNER_API __declspec(dllexport)
    #else
        #define SCANNER_API __declspec(dllimport)
    #endif
#else
    #define SCANNER_API
#endif

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

namespace Scanner {

struct MalwareInfo {
    std::string filePath;
    std::string hash;
    std::string verdict;
};

struct ScanResult {
    size_t totalFilesProcessed;
    size_t malwareFilesDetected;
    size_t errorsCount;
    std::chrono::milliseconds executionTime;
    std::vector<MalwareInfo> detectedMalware;
};

struct ScanSettings {
    std::string rootPath;
    std::string databasePath;
    std::string logPath;
    size_t threadCount = 0;
};

using ProgressCallback = std::function<void(const std::string& currentFile, size_t processedFiles)>;

class SCANNER_API IScanner {
public:
    explicit IScanner() = default;
    virtual ~IScanner() = default;

public:    
    virtual ScanResult Scan(const ScanSettings& settings) = 0;    
    virtual ScanResult ScanWithProgress(const ScanSettings& settings, ProgressCallback callback) = 0;    
    virtual void Stop() = 0;    
    virtual bool IsScanning() const = 0;
};

} // namespace Scanner

extern "C" SCANNER_API Scanner::IScanner* CreateScanner();
extern "C" SCANNER_API void DestroyScanner(Scanner::IScanner* scanner);