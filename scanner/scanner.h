#pragma once

#include "scannerApi.h"

#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

namespace Scanner {
    
class HashDatabase;
class Logger;
class MD5Calculator;
class ThreadPool;

class ScannerImpl : public IScanner {
public:
    ScannerImpl();
    ~ScannerImpl() override;
    
public:
    ScanResult Scan(const ScanSettings& settings) override;
    ScanResult ScanWithProgress(const ScanSettings& settings, ProgressCallback callback) override;
    void Stop() override;
    bool IsScanning() const override;

private:
    void InitializeDependencies(const ScanSettings& settings);
    void ExecuteScan(const ScanSettings& settings);
    void CollectFiles(const std::filesystem::path& root, std::vector<std::filesystem::path>& files);
    void ProcessFile(const std::filesystem::path& filepath);
    
private:
    std::atomic<bool> isScanning_;
    std::atomic<bool> stopRequested_;

    std::atomic<size_t> totalFiles_;
    std::atomic<size_t> malwareFiles_;
    std::atomic<size_t> errors_;
    
    std::unique_ptr<HashDatabase> database_;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<ThreadPool> threadPool_;

    std::vector<MalwareInfo> detectedMalware_;
    std::mutex resultMutex_;

    ProgressCallback progressCallback_;
    std::mutex progressMutex_;
    
    // Время выполнения
    std::chrono::steady_clock::time_point startTime_;
};

} // namespace Scanner