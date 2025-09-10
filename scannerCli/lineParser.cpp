    #include "lineParser.h"

namespace console {
    LineParser::LineParser(console::Config& config) : _config(config) {}

    bool LineParser::parse(int argc, char* argv[]) 
    {
        try
        {
            // Если нет аргументов или только имя программы
            if (argc <= 1) {
                printHelp();
                return false;
            }

            bool hasBase = false;
            bool hasPath = false;

            for (int i = 1; i < argc; ++i) {
                std::string_view arg = argv[i];  
                
                if (arg == "--base" || arg == "-b") {
                    if (i + 1 >= argc) {
                        std::cerr << "Error: --base requires a file path" << std::endl;
                        printHelp();
                        return false;
                    }
                    if (!_config.setPathHashes(argv[i + 1])) {
                        return false;
                    }
                    hasBase = true;
                    ++i;
                } 
                else if (arg == "--log") {
                    if (i + 1 >= argc) {
                        std::cerr << "Error: --log requires a file path" << std::endl;
                        printHelp();
                        return false;
                    }
                    if (!_config.setPathReportLog(argv[i + 1])) {
                        return false;
                    }
                    ++i;
                } 
                else if (arg == "--path" || arg == "-p") {
                    if (i + 1 >= argc) {
                        std::cerr << "Error: --path requires a directory path" << std::endl;
                        printHelp();
                        return false;
                    }
                    if (!_config.setPathScan(argv[i + 1])) {
                        return false;
                    }
                    hasPath = true;
                    ++i;
                } 
                else if (arg == "--help" || arg == "-h") {
                    printHelp();
                    return false;
                } 
                else {
                    std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
                    printHelp();
                    return false;
                }
            }


            if (!hasBase) {
                std::cerr << "Error: Missing required parameter --base" << std::endl;
                printHelp();
                return false;
            }

            if (!hasPath) {
                std::cerr << "Error: Missing required parameter --path" << std::endl;
                printHelp();
                return false;
            }

            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << "[ERROR]: " << e.what() << std::endl;
            printHelp();
            return false;
        }
    }

    std::string_view LineParser::getNextArgument(int i, int argc, char* argv[])
    {
        if (i + 1 >= argc)
            throw std::runtime_error("Incorrect arguments");
        return std::string_view(argv[++i]);
    }

    void LineParser::printHelp()
    {
        std::cout <<
R"(Usage: scanner.exe [OPTIONS]

Options:
    -b, --base <path>     Set base hashes file (must be .csv) [REQUIRED]
    --log <path>          Set log report file [OPTIONAL]
    -p, --path <path>     Set scan directory path [REQUIRED]
    -h, --help            Show this help message and exit

Example:
    scanner.exe --base base.csv --log report.log --path c:\folder

Notes:
    All paths must be valid and accessible.
    Base files must have '.csv' extension.
    --base and --path are required parameters.
)";
    }
}