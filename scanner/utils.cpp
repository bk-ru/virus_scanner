#include "utils.h"

namespace Scanner {
namespace Utils {

bool IsFileReadable(const std::filesystem::path& path) {
    std::ifstream file(path);
    return file.good();
}

size_t GetHardwareConcurrency() {
    size_t cores = std::thread::hardware_concurrency();
    return cores > 0 ? cores : 4;
}

std::string ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Trim(const std::string& str) {
    auto start = str.begin();
    auto end = str.end();
    
    while (start != end && std::isspace(*start)) {
        ++start;
    }
    
    while (start != end && std::isspace(*(end - 1))) {
        --end;
    }
    
    return std::string(start, end);
}

} // namespace Utils
} // namespace Scanner