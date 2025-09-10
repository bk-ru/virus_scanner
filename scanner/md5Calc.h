#pragma once

#include <openssl/md5.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

namespace Scanner {

class MD5Calculator {
public:
    static std::string CalculateFile(const std::filesystem::path& filepath);

private:
    static std::string BytesToHex(const unsigned char* data, size_t len);
};

} // namespace Scanner