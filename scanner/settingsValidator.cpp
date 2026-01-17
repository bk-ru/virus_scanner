#include "settingsValidator.h"
#include "scannerConstants.h"
#include <filesystem>

namespace Scanner {

std::optional<std::string> SettingsValidator::Validate(const ScanSettings& settings) {
    if (auto error = ValidatePath(settings.rootPath)) {
        return error;
    }
    
    if (auto error = ValidateDatabasePath(settings.databasePath)) {
        return error;
    }
    
    if (auto error = ValidateThreadCount(settings.threadCount)) {
        return error;
    }
    
    // Validate log path parent directory exists if path has parent
    if (!settings.logPath.empty()) {
        std::filesystem::path logPath(settings.logPath);
        if (logPath.has_parent_path()) {
            auto parentPath = logPath.parent_path();
            if (!std::filesystem::exists(parentPath)) {
                return "Log file parent directory does not exist: " + parentPath.string();
            }
            if (!std::filesystem::is_directory(parentPath)) {
                return "Log file parent path is not a directory: " + parentPath.string();
            }
        }
    }
    
    return std::nullopt;
}

std::optional<std::string> SettingsValidator::ValidatePath(const std::string& path) {
    if (path.empty()) {
        return "Scan path cannot be empty";
    }
    
    std::filesystem::path fsPath(path);
    
    if (!std::filesystem::exists(fsPath)) {
        return "Scan path does not exist: " + path;
    }
    
    if (!std::filesystem::is_directory(fsPath)) {
        return "Scan path is not a directory: " + path;
    }
    
    // Check if directory is readable
    std::error_code ec;
    auto iter = std::filesystem::directory_iterator(fsPath, ec);
    if (ec) {
        return "Cannot access scan directory: " + path + " (" + ec.message() + ")";
    }
    
    return std::nullopt;
}

std::optional<std::string> SettingsValidator::ValidateDatabasePath(const std::string& path) {
    if (path.empty()) {
        return "Database path cannot be empty";
    }
    
    std::filesystem::path fsPath(path);
    
    if (!std::filesystem::exists(fsPath)) {
        return "Database file does not exist: " + path;
    }
    
    if (!std::filesystem::is_regular_file(fsPath)) {
        return "Database path is not a regular file: " + path;
    }
    
    // Check file extension
    if (fsPath.extension() != ".csv") {
        return "Database file must have .csv extension: " + path;
    }
    
    return std::nullopt;
}

std::optional<std::string> SettingsValidator::ValidateThreadCount(size_t threadCount) {
    // 0 means auto-detect, which is valid
    if (threadCount == 0) {
        return std::nullopt;
    }
    
    if (threadCount < Constants::MIN_THREAD_COUNT) {
        return "Thread count must be at least " + std::to_string(Constants::MIN_THREAD_COUNT);
    }
    
    if (threadCount > Constants::MAX_THREAD_COUNT) {
        return "Thread count cannot exceed " + std::to_string(Constants::MAX_THREAD_COUNT);
    }
    
    return std::nullopt;
}

} // namespace Scanner
