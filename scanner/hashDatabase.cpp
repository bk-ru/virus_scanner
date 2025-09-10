#include "hashDatabase.h"
#include "utils.h"

namespace Scanner {

bool HashDatabase::LoadFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    database_.clear();
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        size_t delimPos = line.find(';');
        if (delimPos == std::string::npos) continue;
        
        std::string hash = Utils::ToLower(Utils::Trim(line.substr(0, delimPos)));
        std::string verdict = Utils::Trim(line.substr(delimPos + 1));
        
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