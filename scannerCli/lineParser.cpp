#include "lineParser.h"
#include <iostream>
#include <stdexcept>

namespace console {

    LineParser::LineParser(console::Config& config) 
    : _config(config) 
    {
    }

    bool LineParser::parse(int argc, char* argv[]) 
    {
        try {
            if (argc <= 1) {
                printHelp();
                return false;
            }

            bool hasBase = false;
            bool hasPath = false;

            for (int i = 1; i < argc; ++i) {
                std::string_view arg = argv[i];

                auto requireNext = [&](const std::string& flag) {
                    if (i + 1 >= argc) {
                        throw std::runtime_error("Option '" + flag + "' requires a value");
                    }
                    return std::string_view(argv[++i]);
                };

                if (arg == "--base" || arg == "-b") {
                    auto value = requireNext("--base");
                    if (!_config.setPathHashes(value.data())) {
                        return false;
                    }
                    hasBase = true;
                }
                else if (arg == "--log") {
                    auto value = requireNext("--log");
                    if (!_config.setPathReportLog(value.data())) {
                        return false;
                    }
                }
                else if (arg == "--path" || arg == "-p") {
                    auto value = requireNext("--path");
                    if (!_config.setPathScan(value.data())) {
                        return false;
                    }
                    hasPath = true;
                }
                else if (arg == "--help" || arg == "-h") {
                    printHelp();
                    return false;
                }
                else {
                    throw std::runtime_error("Unknown option: " + std::string(arg));
                }
            }

            if (!hasBase) {
                throw std::runtime_error("Missing required option: --base");
            }
            if (!hasPath) {
                throw std::runtime_error("Missing required option: --path");
            }

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << "\n";
            printHelp();
            return false;
        }
    }

    std::string_view LineParser::getNextArgument(int i, int argc, char* argv[])
    {
        if (i + 1 >= argc)
            throw std::runtime_error("Missing value after " + std::string(argv[i]));
        return std::string_view(argv[++i]);
    }

    void LineParser::printHelp()
    {
        std::cout <<
R"(Usage: scanner.exe [OPTIONS]

Options:
      --log <path>      Path to log report file
  -b, --base <path>     Path to base hashes file (.csv)
  -p, --path <path>     Directory to scan
  -h, --help            Show help

Example:
  scanner.exe --base base.csv --log report.log --path C:/folder

Notes:
  All paths must be valid and accessible.
  Base file must have '.csv' extension.
  --base and --path are required.
)";
    }

} // namespace console
