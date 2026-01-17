#include "logger.h"
#include <stdexcept>

namespace Scanner {

std::unique_ptr<Logger> Logger::Create(const std::string& logPath) {
    std::ofstream logFile(logPath, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + logPath);
    }
    
    auto logger = std::unique_ptr<Logger>(new Logger(std::move(logFile)));
    logger->WriteSessionHeader();
    return logger;
}

Logger::Logger(std::ofstream&& logFile) 
    : log_file_(std::move(logFile)) {
}

Logger::~Logger() {
    if (log_file_.is_open()) {
        Flush();
        log_file_.close();
    }
}

void Logger::WriteSessionHeader() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        log_file_ << "\n=== Scan started at " << std::ctime(&time_t);
        log_file_ << "===================================\n";
        log_file_.flush();
    }
}

void Logger::LogMalware(const MalwareInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_ << "MALWARE DETECTED:\n";
        log_file_ << "  Path: " << info.filePath << "\n";
        log_file_ << "  Hash: " << info.hash << "\n";
        log_file_ << "  Verdict: " << info.verdict << "\n";
        log_file_ << "---\n";
    }
}

void Logger::LogError(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_ << "ERROR: " << message << "\n";
    }
}

void Logger::LogInfo(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_ << "INFO: " << message << "\n";
    }
}

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_.flush();
    }
}

} // namespace Scanner