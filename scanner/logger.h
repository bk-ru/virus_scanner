#pragma once

#include "scannerApi.h"
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>

namespace Scanner {

class Logger {
public:
    explicit Logger(const std::string& logPath);
    ~Logger();

public:    
    void LogMalware(const MalwareInfo& info);
    void LogError(const std::string& message);
    void Flush();

private:
    std::ofstream logFile_;
    std::mutex mutex_;
};

} // namespace Scanner