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
        bool SetHashDatabasePath(std::string_view path);
        bool SetLogPath(std::string_view path);
        bool SetScanPath(std::string_view path);

    private:
        bool CheckFileExtension(std::string_view path, std::string_view extension) const;
        bool ValidateDirectory(std::string_view path) const;
        bool ValidateFile(std::string_view path) const;
        void PrintDebug(std::string_view prefix, std::string_view value = {}) const;
    
    public:
        const std::string& GetHashDatabasePath() const noexcept;
        const std::string& GetLogPath() const noexcept;
        const std::string& GetScanPath() const noexcept;
    
    private:
        std::string path_hashes_;
        std::string path_report_log_;
        std::string path_scan_;
        bool debug_;
    };
} // namespace console