#pragma once

#include <cstddef>

namespace Scanner {
namespace Constants {

// File processing limits
constexpr size_t MAX_FILE_SIZE = 100 * 1024 * 1024;  // 100 MB
constexpr size_t MAX_PATH_DEPTH = 100;
constexpr size_t MIN_THREAD_COUNT = 1;
constexpr size_t MAX_THREAD_COUNT = 256;

// Hash calculation
constexpr size_t HASH_BUFFER_SIZE = 64 * 1024;  // 64 KB

// Database limits
constexpr size_t MAX_DATABASE_ENTRIES = 10'000'000;
constexpr char CSV_DELIMITER = ';';
constexpr size_t MD5_HASH_LENGTH = 32;

} // namespace Constants
} // namespace Scanner
