#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <thread>
#include <algorithm>
#include <cctype>

namespace Scanner {
    
// Вспомогательные функции
namespace Utils {
    // Проверка доступа к файлу для чтения
    bool IsFileReadable(const std::filesystem::path& path);   
    // Получение количества процессорных ядер
    size_t GetHardwareConcurrency();
    
    std::string ToLower(const std::string& str);
    std::string Trim(const std::string& str);
}

} // namespace Scanner