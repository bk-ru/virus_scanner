#include "config.h"
#include "lineParser.h"

int main(int argc, char* argv[]) {
    console::Config config(true);
    console::LineParser lineParser(config);
    lineParser.parse(argc, argv);
    return 0;
}