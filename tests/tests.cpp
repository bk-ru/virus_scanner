#include <gtest/gtest.h>
#include "scannerApi.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "integration_test";
        fs::create_directories(testDir);        
        scanDir = testDir / "scan";
        subDir = scanDir / "subdir";
        fs::create_directories(subDir);        
        hashFile = testDir / "hashes.csv";
        logFile = testDir / "test.log";  

        CreateTestFile("clean.txt", "This is clean file");
        CreateTestFile("malware1.txt", "Hello, World!");  // MD5: 65a8e27d8879283831b664bd8b7f0ad4
        CreateTestFile("subdir/malware2.txt", "");        // MD5: d41d8cd98f00b204e9800998ecf8427e

        std::ofstream hashDb(hashFile);
        hashDb << "65a8e27d8879283831b664bd8b7f0ad4;TestMalware1\n";
        hashDb << "d41d8cd98f00b204e9800998ecf8427e;TestMalware2\n";
        hashDb.close();
    }
    
    void TearDown() override {
        std::error_code ec;
        fs::remove_all(testDir, ec);
    }
    
    void CreateTestFile(const std::string& relativePath, const std::string& content) {
        auto filePath = scanDir / relativePath;
        fs::create_directories(filePath.parent_path());
        std::ofstream file(filePath, std::ios::binary);
        file.write(content.data(), content.size());
        file.close();
    }
    
protected:
    fs::path testDir;
    fs::path scanDir;
    fs::path subDir;
    fs::path hashFile;
    fs::path logFile;
};

TEST_F(IntegrationTest, FullScanWithMalware) {
    auto scanner = std::unique_ptr<Scanner::IScanner>(CreateScanner());
    ASSERT_NE(scanner, nullptr);

    Scanner::ScanSettings settings;
    settings.rootPath = scanDir.string();
    settings.databasePath = hashFile.string();
    settings.logPath = logFile.string();
    settings.threadCount = 2;
    
    Scanner::ScanResult result = scanner->Scan(settings);
    EXPECT_EQ(result.totalFilesProcessed, 3);
    EXPECT_EQ(result.malwareFilesDetected, 2);
    EXPECT_EQ(result.errorsCount, 0);
    EXPECT_GT(result.executionTime.count(), 0);
    EXPECT_EQ(result.detectedMalware.size(), 2);
    EXPECT_TRUE(fs::exists(logFile));
    DestroyScanner(scanner.release());
}

TEST_F(IntegrationTest, ScanWithProgress) {
    auto scanner = std::unique_ptr<Scanner::IScanner>(CreateScanner());
    ASSERT_NE(scanner, nullptr);    
    Scanner::ScanSettings settings;
    settings.rootPath = scanDir.string();
    settings.databasePath = hashFile.string();
    settings.logPath = logFile.string();
    settings.threadCount = 1;
    
    size_t progressCallbacks = 0;
    auto progressCallback = [&progressCallbacks](const std::string& file, size_t processed) {
        progressCallbacks++;
        EXPECT_FALSE(file.empty());
        EXPECT_GT(processed, 0);
    };
    
    Scanner::ScanResult result = scanner->ScanWithProgress(settings, progressCallback);    
    EXPECT_GT(progressCallbacks, 0);
    EXPECT_EQ(result.malwareFilesDetected, 2);    
    DestroyScanner(scanner.release());
}

TEST_F(IntegrationTest, EmptyDirectory) {
    auto emptyDir = testDir / "empty";
    fs::create_directories(emptyDir);    
    auto scanner = std::unique_ptr<Scanner::IScanner>(CreateScanner());
    ASSERT_NE(scanner, nullptr);   

    Scanner::ScanSettings settings;
    settings.rootPath = emptyDir.string();
    settings.databasePath = hashFile.string();
    settings.logPath = logFile.string();
    settings.threadCount = 1;    
    Scanner::ScanResult result = scanner->Scan(settings);
    
    EXPECT_EQ(result.totalFilesProcessed, 0);
    EXPECT_EQ(result.malwareFilesDetected, 0);
    EXPECT_EQ(result.errorsCount, 0);    
    DestroyScanner(scanner.release());
}

TEST_F(IntegrationTest, InvalidDatabaseFile) {
    auto scanner = std::unique_ptr<Scanner::IScanner>(CreateScanner());
    ASSERT_NE(scanner, nullptr);
    
    Scanner::ScanSettings settings;
    settings.rootPath = scanDir.string();
    settings.databasePath = "nonexistent.csv";
    settings.logPath = logFile.string();
    settings.threadCount = 1;
    
    Scanner::ScanResult result = scanner->Scan(settings);
    EXPECT_GT(result.errorsCount, 0);    
    DestroyScanner(scanner.release());
}