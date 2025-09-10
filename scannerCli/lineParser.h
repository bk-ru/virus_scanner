#pragma once

#include "config.h"

namespace console 
{
    class LineParser {
    public:
        LineParser(console::Config& config);
        ~LineParser() = default;

    public:
        bool parse(int argc, char* argv[]);
        std::string_view getNextArgument(int i, int argc, char* argv[]);

    public:
        void printHelp();

    private:
        console::Config& _config;    
    };
} // namespace console