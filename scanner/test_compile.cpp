#include "scannerApi.h"
#include <iostream>

int main() {
    Scanner::IScanner* scanner = CreateScanner();
    if (!scanner) {
        std::cerr << "Failed to create scanner!\n";
        return 1;
    }

    Scanner::ScanSettings settings = {
        "C:/github/virus",
        "C:/github/virus_scanner/base.csv",
        "C:/github/virus_scanner/log.txt",
        4
    };

    auto result = scanner->Scan(settings);
    std::cout << "Total: " << result.totalFilesProcessed << "\n";
    std::cout << "malwareFilesDetected: " << result.malwareFilesDetected << "\n";
    std::cout << "errorsCount: " << result.errorsCount << "\n";
    std::cout << "executionTime: " << result.executionTime << "\n";

    DestroyScanner(scanner);
    return 0;
}
