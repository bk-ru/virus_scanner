#include "md5Calc.h"

namespace Scanner {

std::string MD5Calculator::CalculateFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath.string());
    }
    
    MD5_CTX md5Context;
    MD5_Init(&md5Context);
    
    const auto fileSize = std::filesystem::file_size(filepath);
    const size_t bufferSize = std::min(
        static_cast<size_t>(1024 * 1024), // 1MB
        static_cast<size_t>(fileSize > 0 ? fileSize : 1024)
    );
    std::vector<char> buffer(bufferSize);
    
    while (file.read(buffer.data(), bufferSize) || file.gcount() > 0)
        MD5_Update(&md5Context, buffer.data(), static_cast<size_t>(file.gcount()));
    
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &md5Context);    
    return BytesToHex(result, MD5_DIGEST_LENGTH);
}

std::string MD5Calculator::BytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<unsigned>(data[i]);
    }
    return ss.str();
}

} // namespace Scanner