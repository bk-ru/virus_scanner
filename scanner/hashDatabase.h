#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include <fstream>
#include <sstream>

namespace Scanner {

class HashDatabase {
public:
    bool LoadFromCSV(const std::string& filepath);
    bool IsMalicious(const std::string& hash, std::string& verdict) const;
    size_t GetSize() const { return database_.size(); }

private:
    std::unordered_map<std::string, std::string> database_;
    mutable std::mutex mutex_;
};

} // namespace Scanner