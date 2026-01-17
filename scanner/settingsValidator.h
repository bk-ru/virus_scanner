#pragma once

#include "scannerApi.h"
#include <string>
#include <optional>

namespace Scanner {

class SettingsValidator {
public:
    // Returns error message if validation fails, std::nullopt if valid
    static std::optional<std::string> Validate(const ScanSettings& settings);

private:
    static std::optional<std::string> ValidatePath(const std::string& path);
    static std::optional<std::string> ValidateDatabasePath(const std::string& path);
    static std::optional<std::string> ValidateThreadCount(size_t threadCount);
};

} // namespace Scanner
