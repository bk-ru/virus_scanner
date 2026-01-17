#include <gtest/gtest.h>
#include "settingsValidator.h"
#include "hashDatabase.h"
#include "utils.h"
#include "scannerConstants.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// ============================================================================
// SettingsValidator Tests
// ============================================================================

class SettingsValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "validator_test";
        fs::create_directories(testDir);
        
        validDir = testDir / "valid_dir";
        fs::create_directories(validDir);
        
        validCsv = testDir / "valid.csv";
        std::ofstream(validCsv) << "abc123;Malware\n";
    }
    
    void TearDown() override {
        std::error_code ec;
        fs::remove_all(testDir, ec);
    }
    
    fs::path testDir;
    fs::path validDir;
    fs::path validCsv;
};

TEST_F(SettingsValidatorTest, ValidSettings) {
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = validCsv.string();
    settings.logPath = (testDir / "log.txt").string();
    settings.threadCount = 4;
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    EXPECT_FALSE(error.has_value());
}

TEST_F(SettingsValidatorTest, EmptyScanPath) {
    Scanner::ScanSettings settings;
    settings.rootPath = "";
    settings.databasePath = validCsv.string();
    settings.logPath = "log.txt";
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("empty"), std::string::npos);
}

TEST_F(SettingsValidatorTest, NonExistentScanPath) {
    Scanner::ScanSettings settings;
    settings.rootPath = (testDir / "nonexistent").string();
    settings.databasePath = validCsv.string();
    settings.logPath = "log.txt";
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("does not exist"), std::string::npos);
}

TEST_F(SettingsValidatorTest, ScanPathIsFile) {
    Scanner::ScanSettings settings;
    settings.rootPath = validCsv.string();  // File, not directory
    settings.databasePath = validCsv.string();
    settings.logPath = "log.txt";
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("not a directory"), std::string::npos);
}

TEST_F(SettingsValidatorTest, NonExistentDatabasePath) {
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = (testDir / "nonexistent.csv").string();
    settings.logPath = "log.txt";
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("does not exist"), std::string::npos);
}

TEST_F(SettingsValidatorTest, InvalidDatabaseExtension) {
    auto txtFile = testDir / "database.txt";
    std::ofstream(txtFile) << "data\n";
    
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = txtFile.string();
    settings.logPath = "log.txt";
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find(".csv"), std::string::npos);
}

TEST_F(SettingsValidatorTest, ThreadCountTooHigh) {
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = validCsv.string();
    settings.logPath = "log.txt";
    settings.threadCount = 1000;  // Exceeds MAX_THREAD_COUNT
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("cannot exceed"), std::string::npos);
}

TEST_F(SettingsValidatorTest, ThreadCountZeroIsValid) {
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = validCsv.string();
    settings.logPath = "log.txt";
    settings.threadCount = 0;  // Auto-detect
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    EXPECT_FALSE(error.has_value());
}

TEST_F(SettingsValidatorTest, LogPathParentDoesNotExist) {
    Scanner::ScanSettings settings;
    settings.rootPath = validDir.string();
    settings.databasePath = validCsv.string();
    settings.logPath = (testDir / "nonexistent_dir" / "log.txt").string();
    
    auto error = Scanner::SettingsValidator::Validate(settings);
    ASSERT_TRUE(error.has_value());
    EXPECT_NE(error->find("parent directory does not exist"), std::string::npos);
}

// ============================================================================
// HashDatabase Tests
// ============================================================================

class HashDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "hashdb_test";
        fs::create_directories(testDir);
    }
    
    void TearDown() override {
        std::error_code ec;
        fs::remove_all(testDir, ec);
    }
    
    void CreateCSV(const std::string& filename, const std::string& content) {
        std::ofstream file(testDir / filename);
        file << content;
    }
    
    fs::path testDir;
};

TEST_F(HashDatabaseTest, LoadValidCSV) {
    CreateCSV("valid.csv", 
        "abc123def456789012345678901234ab;Trojan\n"
        "def456abc789012345678901234567cd;Virus\n");
    
    Scanner::HashDatabase db;
    EXPECT_TRUE(db.LoadFromCSV((testDir / "valid.csv").string()));
    EXPECT_EQ(db.GetSize(), 2);
}

TEST_F(HashDatabaseTest, LoadNonExistentFile) {
    Scanner::HashDatabase db;
    EXPECT_FALSE(db.LoadFromCSV((testDir / "nonexistent.csv").string()));
}

TEST_F(HashDatabaseTest, LoadEmptyFile) {
    CreateCSV("empty.csv", "");
    
    Scanner::HashDatabase db;
    EXPECT_FALSE(db.LoadFromCSV((testDir / "empty.csv").string()));
}

TEST_F(HashDatabaseTest, SkipMalformedLines) {
    CreateCSV("malformed.csv",
        "abc123def456789012345678901234ab;Trojan\n"
        "no_delimiter_here\n"  // Should be skipped
        "def456abc789012345678901234567cd;Virus\n"
        ";no_hash\n"  // Should be skipped
        "no_verdict;\n");  // Should be skipped
    
    Scanner::HashDatabase db;
    EXPECT_TRUE(db.LoadFromCSV((testDir / "malformed.csv").string()));
    EXPECT_EQ(db.GetSize(), 2);  // Only 2 valid entries
}

TEST_F(HashDatabaseTest, SkipInvalidHashLength) {
    CreateCSV("invalid_hash.csv",
        "abc123def456789012345678901234ab;Trojan\n"  // Valid: 32 chars
        "short;Virus\n"  // Invalid: too short
        "toolongabc123def456789012345678901234abcd;Malware\n");  // Invalid: too long
    
    Scanner::HashDatabase db;
    EXPECT_TRUE(db.LoadFromCSV((testDir / "invalid_hash.csv").string()));
    EXPECT_EQ(db.GetSize(), 1);  // Only 1 valid entry
}

TEST_F(HashDatabaseTest, SkipNonHexHash) {
    CreateCSV("non_hex.csv",
        "abc123def456789012345678901234ab;Trojan\n"  // Valid
        "xyz123def456789012345678901234ab;Virus\n");  // Invalid: contains 'xyz'
    
    Scanner::HashDatabase db;
    EXPECT_TRUE(db.LoadFromCSV((testDir / "non_hex.csv").string()));
    EXPECT_EQ(db.GetSize(), 1);  // Only 1 valid entry
}

TEST_F(HashDatabaseTest, IsMaliciousFindsHash) {
    CreateCSV("test.csv", "abc123def456789012345678901234ab;TestMalware\n");
    
    Scanner::HashDatabase db;
    db.LoadFromCSV((testDir / "test.csv").string());
    
    std::string verdict;
    EXPECT_TRUE(db.IsMalicious("abc123def456789012345678901234ab", verdict));
    EXPECT_EQ(verdict, "TestMalware");
}

TEST_F(HashDatabaseTest, IsMaliciousCaseInsensitive) {
    CreateCSV("test.csv", "abc123def456789012345678901234ab;TestMalware\n");
    
    Scanner::HashDatabase db;
    db.LoadFromCSV((testDir / "test.csv").string());
    
    std::string verdict;
    EXPECT_TRUE(db.IsMalicious("ABC123DEF456789012345678901234AB", verdict));
    EXPECT_EQ(verdict, "TestMalware");
}

TEST_F(HashDatabaseTest, IsMaliciousNotFound) {
    CreateCSV("test.csv", "abc123def456789012345678901234ab;TestMalware\n");
    
    Scanner::HashDatabase db;
    db.LoadFromCSV((testDir / "test.csv").string());
    
    std::string verdict;
    EXPECT_FALSE(db.IsMalicious("000000000000000000000000000000000", verdict));
}

TEST_F(HashDatabaseTest, TrimWhitespace) {
    CreateCSV("whitespace.csv",
        "  abc123def456789012345678901234ab  ;  Trojan  \n"
        "def456abc789012345678901234567cd;Virus\n");
    
    Scanner::HashDatabase db;
    EXPECT_TRUE(db.LoadFromCSV((testDir / "whitespace.csv").string()));
    
    std::string verdict;
    EXPECT_TRUE(db.IsMalicious("abc123def456789012345678901234ab", verdict));
    EXPECT_EQ(verdict, "Trojan");
}

// ============================================================================
// Utils Tests
// ============================================================================

TEST(UtilsTest, ToLowerConvertsCorrectly) {
    EXPECT_EQ(Scanner::Utils::ToLower("HELLO"), "hello");
    EXPECT_EQ(Scanner::Utils::ToLower("HeLLo"), "hello");
    EXPECT_EQ(Scanner::Utils::ToLower("hello"), "hello");
    EXPECT_EQ(Scanner::Utils::ToLower("ABC123"), "abc123");
    EXPECT_EQ(Scanner::Utils::ToLower(""), "");
}

TEST(UtilsTest, TrimRemovesWhitespace) {
    EXPECT_EQ(Scanner::Utils::Trim("  hello  "), "hello");
    EXPECT_EQ(Scanner::Utils::Trim("hello"), "hello");
    EXPECT_EQ(Scanner::Utils::Trim("  hello"), "hello");
    EXPECT_EQ(Scanner::Utils::Trim("hello  "), "hello");
    EXPECT_EQ(Scanner::Utils::Trim("   "), "");
    EXPECT_EQ(Scanner::Utils::Trim(""), "");
}

TEST(UtilsTest, TrimHandlesTabs) {
    EXPECT_EQ(Scanner::Utils::Trim("\thello\t"), "hello");
    EXPECT_EQ(Scanner::Utils::Trim("\t\thello"), "hello");
}

TEST(UtilsTest, GetHardwareConcurrency) {
    size_t cores = Scanner::Utils::GetHardwareConcurrency();
    EXPECT_GT(cores, 0);
    EXPECT_LE(cores, 1024);  // Sanity check
}

// ============================================================================
// Constants Tests
// ============================================================================

TEST(ConstantsTest, ValidValues) {
    EXPECT_GT(Scanner::Constants::MAX_FILE_SIZE, 0);
    EXPECT_GT(Scanner::Constants::MAX_PATH_DEPTH, 0);
    EXPECT_GT(Scanner::Constants::MIN_THREAD_COUNT, 0);
    EXPECT_GT(Scanner::Constants::MAX_THREAD_COUNT, Scanner::Constants::MIN_THREAD_COUNT);
    EXPECT_GT(Scanner::Constants::HASH_BUFFER_SIZE, 0);
    EXPECT_EQ(Scanner::Constants::CSV_DELIMITER, ';');
    EXPECT_EQ(Scanner::Constants::MD5_HASH_LENGTH, 32);
}
