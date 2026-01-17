#include "config.h"

namespace console
{
Config::Config(bool debug) : debug_(debug) {}

bool Config::SetHashDatabasePath(std::string_view path)
{
    if (!CheckFileExtension(path, ".csv")) {
        std::cerr << "[ERROR]: " << path 
                    << " - The file extension must be .csv" << std::endl;
        return false;
    }

    if (!ValidateFile(path)) {
        std::cerr << "[ERROR]: " << path 
                    << " - File does not exist or is not accessible" << std::endl;
        return false;
    }

    PrintDebug("SetHashDatabasePath: ", path);
    path_hashes_ = path;
    return true;
}

bool Config::SetLogPath(std::string_view path) 
{
    fs::path logPath(path);
    if (logPath.has_parent_path() && !fs::exists(logPath.parent_path())) {
        std::cerr << "[ERROR]: Directory for log file does not exist: " 
                    << logPath.parent_path() << std::endl;
        return false;
    }

    PrintDebug("SetLogPath: ", path);
    path_report_log_ = path;
    return true;
}

bool Config::SetScanPath(std::string_view path) 
{
    if (!ValidateDirectory(path)) {
        std::cerr << "[ERROR]: " << path 
                    << " - Directory does not exist or is not accessible" << std::endl;
        return false;
    }

    PrintDebug("SetScanPath: ", path);
    path_scan_ = path;
    return true;
}

bool Config::CheckFileExtension(std::string_view path, std::string_view extension) const
{
    fs::path filePath(path);
    return filePath.extension() == extension;
}

void Config::PrintDebug(std::string_view prefix, std::string_view value) const
{
    if (debug_) {
        std::cout << "[DEBUG]: " << prefix;
        if (!value.empty()) 
            std::cout << value;
        std::cout << std::endl;
    }
}

bool Config::ValidateDirectory(std::string_view path) const 
{
    fs::path p(path);
    return fs::exists(p) && fs::is_directory(p);
}

bool Config::ValidateFile(std::string_view path) const 
{
    fs::path p(path);
    return fs::exists(p) && fs::is_regular_file(p);
}

const std::string& Config::GetHashDatabasePath() const noexcept { return path_hashes_; }
const std::string& Config::GetLogPath() const noexcept { return path_report_log_; }
const std::string& Config::GetScanPath() const noexcept { return path_scan_; }

} // namespace console