#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace console 
{
    class Config {
    public:
        Config(bool debug = false);
        ~Config() = default;

    public:
        bool setPathHashes(std::string_view pathHashes);
        bool setPathReportLog(std::string_view pathReportLog);
        bool setPathScan(std::string_view pathScan);

    private:
        bool checkFileExtension(std::string_view path, std::string_view extension);
        inline bool validateDirectory(std::string_view path) const;
        inline bool validateFile(std::string_view path) const;

    public:
        std::string_view getPathHashes() const noexcept;
        std::string_view getPathReportLog() const noexcept;
        std::string_view getPathScan() const noexcept;

    private:
        void printDebug(std::string_view prefix, std::string_view value = {});
    
    private:
        std::string_view _pathHashes;
        std::string_view _pathReportLog;
        std::string_view _pathScan;

        bool _debug;
    };
} // namespace console