#pragma once

#include "scannerApi.h"
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <memory>

namespace Scanner {

class Logger {
public:
    // Factory method to create logger with proper error handling
    static std::unique_ptr<Logger> Create(const std::string& logPath);
    
    ~Logger();

    void LogMalware(const MalwareInfo& info);
    void LogError(const std::string& message);
    void LogInfo(const std::string& message);
    void Flush();

private:
    // Private constructor - use Create() factory method
    explicit Logger(std::ofstream&& logFile);
    
    void WriteSessionHeader();

private:
    std::ofstream log_file_;
    std::mutex mutex_;
};

} // namespace Scanner