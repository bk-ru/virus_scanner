#include "hashDatabase.h"
#include "utils.h"
#include "scannerConstants.h"
#include <algorithm>
#include <cctype>

namespace Scanner {

bool HashDatabase::LoadFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    database_.clear();
    size_t lineCount = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        // Check database size limit
        if (++lineCount > Constants::MAX_DATABASE_ENTRIES) {
            return false;  // Database too large
        }
        
        size_t delimPos = line.find(Constants::CSV_DELIMITER);
        if (delimPos == std::string::npos) {
            continue;  // Skip malformed lines
        }
        
        std::string hash = Utils::ToLower(Utils::Trim(line.substr(0, delimPos)));
        std::string verdict = Utils::Trim(line.substr(delimPos + 1));
        
        // Validate hash format (MD5 should be 32 hex characters)
        if (hash.length() != Constants::MD5_HASH_LENGTH) {
            continue;  // Skip invalid hash
        }
        
        if (!std::all_of(hash.begin(), hash.end(), [](unsigned char c) {
            return std::isxdigit(c);
        })) {
            continue;  // Skip non-hex hash
        }
        
        if (!hash.empty() && !verdict.empty()) {
            database_[hash] = verdict;
        }
    }
    
    return !database_.empty();
}

bool HashDatabase::IsMalicious(const std::string& hash, std::string& verdict) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = database_.find(Utils::ToLower(hash));
    if (it != database_.end()) {
        verdict = it->second;
        return true;
    }
    return false;
}

} // namespace Scanner