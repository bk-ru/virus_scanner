#include "config.h"

namespace console
{
Config::Config(bool debug) : _debug(debug) {}

bool Config::setPathHashes(std::string_view pathHashes)
{
    if (!checkFileExtension(pathHashes, ".csv")) {
        std::cerr << "[ERROR]: " << pathHashes 
                    << " - The file extension must be .csv" << std::endl;
        return false;
    }

    if (!validateFile(pathHashes)) {
        std::cerr << "[ERROR]: " << pathHashes 
                    << " - File does not exist or is not accessible" << std::endl;
        return false;
    }

    printDebug("setPathHashes: ", pathHashes);
    _pathHashes = pathHashes;
    return true;
}

bool Config::setPathReportLog(std::string_view pathReportLog) 
{
    fs::path logPath(pathReportLog);
    if (logPath.has_parent_path() && !fs::exists(logPath.parent_path())) {
        std::cerr << "[ERROR]: Directory for log file does not exist: " 
                    << logPath.parent_path() << std::endl;
        return false;
    }

    printDebug("setPathReportLog: ", pathReportLog);
    _pathReportLog = pathReportLog;
    return true;
}

bool Config::setPathScan(std::string_view pathScan) 
{
    if (!validateDirectory(pathScan)) {
        std::cerr << "[ERROR]: " << pathScan 
                    << " - Directory does not exist or is not accessible" << std::endl;
        return false;
    }

    printDebug("setPathScan: ", pathScan);
    _pathScan = pathScan;
    return true;
}

bool Config::checkFileExtension(std::string_view path, std::string_view extension)
{
    fs::path filePath = path;
    bool flag = false;
    if (filePath.extension() == extension)
        flag = true;

    return flag;
}

void Config::printDebug(std::string_view prefix, std::string_view value)
{
    if (_debug) {
        std::cout << "[DEBUG]: " << prefix;
        if (!value.empty()) 
            std::cout << value;
        std::cout << std::endl;
    }
}

inline bool Config::validateDirectory(std::string_view path) const 
{
    fs::path p(path);
    return fs::exists(p) && fs::is_directory(p);
}

inline bool Config::validateFile(std::string_view path) const 
{
    fs::path p(path);
    return fs::exists(p) && fs::is_regular_file(p);
}

std::string_view Config::getPathHashes() const noexcept  { return _pathHashes; }
std::string_view Config::getPathReportLog() const noexcept  { return _pathReportLog; }
std::string_view Config::getPathScan() const noexcept  { return _pathScan; }

} // namespace console