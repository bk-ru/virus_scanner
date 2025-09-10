#include "logger.h"

namespace Scanner {

Logger::Logger(const std::string& logPath) {
    logFile_.open(logPath, std::ios::out | std::ios::app);
    if (logFile_.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        logFile_ << "\n=== Scan started at " << std::ctime(&time_t);
        logFile_ << "===================================\n";
    }
}

Logger::~Logger() {
    if (logFile_.is_open()) {
        Flush();
        logFile_.close();
    }
}

void Logger::LogMalware(const MalwareInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_ << "MALWARE DETECTED:\n";
        logFile_ << "  Path: " << info.filePath << "\n";
        logFile_ << "  Hash: " << info.hash << "\n";
        logFile_ << "  Verdict: " << info.verdict << "\n";
        logFile_ << "---\n";
    }
}

void Logger::LogError(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open()) {
        logFile_ << "ERROR: " << message << "\n";
    }
}

void Logger::Flush() {
    if (logFile_.is_open()) {
        logFile_.flush();
    }
}

} // namespace Scanner