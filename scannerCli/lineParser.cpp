    #include "lineParser.h"

    namespace console {
        LineParser::LineParser(console::Config& config) : _config(config) {}

        bool LineParser::parse(int argc, char* argv[]) 
        {
            try
            {
                for (int i = 0; i < argc; ++i) {
                    std::string_view arg = argv[i];  
                    if (arg == "--base" || arg == "-b") {
                        _config.setPathHashes(getNextArgument(i, argc, argv));
                    } else if (arg == "--log") {
                        _config.setPathReportLog(getNextArgument(i, argc, argv));
                    } else if (arg == "--path" || arg == "-p") {
                        _config.setPathScan(getNextArgument(i, argc, argv));
                    } else if (arg == "--help" || arg == "-h") {
                        printHelp();
                    }
                }
                return true;
            }
            catch(const std::exception& e)
            {
                std::cerr << "[ERROR]: " << e.what() << std::endl;
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
            -b, --base <path>     Set base hashes file (must be .csv)
            --log <path>          Set log report file
            -p, --path <path>     Set scan directory path
            -h, --help            Show this help message and exit

        Example:
            scanner.exe --base base.csv --log report.log --path c:\folder

        Notes:
            All paths must be valid and accessible.
            Base files must have '.csv' extension.
    )";
        }
    }