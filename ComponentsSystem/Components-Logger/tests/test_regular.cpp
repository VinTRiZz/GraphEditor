#include <gtest/gtest.h>

#include <Components/Logger/Logger.h>

#include <fstream>
#include <filesystem>
#include <regex>
#include <thread>

namespace {

using StreamPair_t = std::pair<std::shared_ptr<std::ostringstream>, std::streambuf*>;

StreamPair_t startCaptureOutput(decltype(std::cout)& stream) {
    auto pOss = std::make_shared<std::ostringstream>();
    std::streambuf* old_buf = stream.rdbuf();
    stream.rdbuf(pOss->rdbuf());
    return {pOss, old_buf};
}

std::string getCapturedOutput(StreamPair_t& captureId) {
    std::cout.rdbuf(captureId.second);
    return captureId.first->str();
}

void checkPattern(const std::string& captured, const std::string& pattern) {
    auto patternStartIt = std::search(
            captured.begin(), captured.end(),
            pattern.begin(), pattern.end());
    ASSERT_NE(captured.end(), patternStartIt)
        << "Expected pattern not found \n======================\n\t" << pattern
        << "\nin data:\n======================\n" << captured
        << "\n======================";
}

}

TEST(Components_Logger, SetLogdir) {
    const std::string testDirpath {"test"};
    if (std::filesystem::exists(testDirpath)) {
        std::filesystem::remove_all(testDirpath);
    }
    auto direcreateRes = std::filesystem::create_directory(testDirpath);
    ASSERT_TRUE(direcreateRes);
    COMPLOG_SET_LOGSDIR(testDirpath);

    const std::string testString {"TestString"};
    auto coutCollectorId = startCaptureOutput(std::cout);
    auto cerrCollectorId = startCaptureOutput(std::cerr);

    // Check STDOUT
    COMPLOG_SYNC_DEBUG  (testString, "OUT-DEBUG", 1);
    COMPLOG_SYNC_INFO   (testString, "OUT-INFO", 2);
    COMPLOG_SYNC_OK     (testString, "OUT-OK", 3);

    // Check STDERR
    COMPLOG_SYNC_WARNING(testString, "OUT-WARNING", 4);
    COMPLOG_SYNC_ERROR  (testString, "OUT-ERROR", 5);

    ASSERT_TRUE(std::filesystem::exists(COMPLOG_GET_LOGFILE())) << "Logging file did not created. File path:" << COMPLOG_GET_LOGFILE();

    // Check if written
    std::ifstream logfileReader(COMPLOG_GET_LOGFILE().data());
    ASSERT_TRUE(logfileReader.is_open()) << "Failed to open log file";
    std::string iStr;
    iStr.resize(2048);
    auto readSize = logfileReader.readsome(iStr.data(), iStr.size());
    iStr.resize(readSize);

    std::regex rexp(testString);
    auto writtenBeg = std::sregex_iterator(iStr.begin(), iStr.end(), rexp);
    auto writtenEnd = std::sregex_iterator();
    auto writtenCount = std::distance(writtenBeg, writtenEnd);
    ASSERT_EQ(writtenCount, 5) << "Invalid count of data strings written into file";

    std::filesystem::remove_all(testDirpath);

    // For logger message sending
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Components_Logger, ModeOutput) {
    const std::string testDirpath {"test"};
    if (std::filesystem::exists(testDirpath)) {
        std::filesystem::remove_all(testDirpath);
    }
    auto direcreateRes = std::filesystem::create_directory(testDirpath);
    ASSERT_TRUE(direcreateRes);
    COMPLOG_SET_LOGSDIR(testDirpath);

    const std::string testString {"TestString"};
    auto coutCollectorId = startCaptureOutput(std::cout);
    auto cerrCollectorId = startCaptureOutput(std::cerr);

    // Check STDOUT
    COMPLOG_SYNC_DEBUG  (testString, "OUT-DEBUG", 1);
    COMPLOG_SYNC_INFO   (testString, "OUT-INFO", 2);
    COMPLOG_SYNC_OK     (testString, "OUT-OK", 3);
    auto expectedDebugPattern = testString + " OUT-DEBUG " + std::to_string(1);
    auto expectedInfoPattern = testString + " OUT-INFO " + std::to_string(2);
    auto expectedOkPattern = testString + " OUT-OK " + std::to_string(3);
    {
        auto captured = getCapturedOutput(coutCollectorId);
        auto lineCount = std::count(captured.begin(), captured.end(), '\n');
        ASSERT_NE(captured.size(), 0) << "Nothing printed to STDERR";
        ASSERT_EQ(3, lineCount) << "Invalid printed lines";

        checkPattern(captured, expectedDebugPattern);
        checkPattern(captured, expectedInfoPattern);
        checkPattern(captured, expectedOkPattern);
    }

    // Check STDERR
    COMPLOG_SYNC_WARNING(testString, "OUT-WARNING", 4);
    COMPLOG_SYNC_ERROR  (testString, "OUT-ERROR", 5);
    auto expectedWarningPattern = testString + " OUT-WARNING " + std::to_string(4);
    auto expectedErrorPattern = testString + " OUT-ERROR " + std::to_string(5);
    {
        auto captured = getCapturedOutput(cerrCollectorId);
        ASSERT_NE(captured.size(), 0) << "Nothing printed to STDERR";

        auto lineCount = std::count(captured.begin(), captured.end(), '\n');
        ASSERT_EQ(2, lineCount) << "Invalid printed line count";

        checkPattern(captured, expectedWarningPattern);
        checkPattern(captured, expectedErrorPattern);
    }

    // For logger message writing
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::ifstream loggerFile(std::string(COMPLOG_GET_LOGFILE()));
        auto captured = std::string(std::istreambuf_iterator<char>(loggerFile),
                                    std::istreambuf_iterator<char>());

        checkPattern(captured, expectedDebugPattern);
        checkPattern(captured, expectedInfoPattern);
        checkPattern(captured, expectedOkPattern);
        checkPattern(captured, expectedWarningPattern);
        checkPattern(captured, expectedErrorPattern);
    }

    std::filesystem::remove_all(testDirpath);
}

TEST(Components_Logger, ArrayProcessing) {
    const std::string testDirpath {"test"};
    if (std::filesystem::exists(testDirpath)) {
        std::filesystem::remove_all(testDirpath);
    }
    auto direcreateRes = std::filesystem::create_directory(testDirpath);
    ASSERT_TRUE(direcreateRes);
    COMPLOG_SET_LOGSDIR(testDirpath);

    std::string testExample {"Test string"};

    std::vector<std::string> testVect {
        "Values", "are", "not", "same"
    };

    std::map<int, std::string> testMap {
        {2, "values"},
        {4, "the"},
        {5, "map"},
        {3, "of"},
        {1, "Example"},
};

    // Types checking (vector)
    using vecttyp = std::decay_t<decltype(testVect)>;
    ASSERT_FALSE(Logger::IsString<vecttyp>::value)      << "Vector must not be string";
    ASSERT_TRUE(Logger::IsContainer<vecttyp>::value)    << "Vector must be container";
    ASSERT_FALSE(Logger::IsMap<vecttyp>::value)         << "Vector must not be map";

    // Check vector output
    auto coutCollectorId = startCaptureOutput(std::cout);
    COMPLOG_SYNC_INFO("vector:", testVect);
    auto captured = getCapturedOutput(coutCollectorId);
    checkPattern(captured, "vector: { Values | are | not | same }");
    captured.clear();

    // ======================================= //

    // Types checking (map)
    using maptyp = std::decay_t<decltype(testMap)>;
    ASSERT_FALSE(Logger::IsString<maptyp>::value)      << "Map must not be string";
    ASSERT_TRUE(Logger::IsContainer<maptyp>::value)    << "Map must be container";
    ASSERT_TRUE(Logger::IsMap<maptyp>::value)         << "Map must be map";

    // Check map output
    coutCollectorId = startCaptureOutput(std::cout);
    COMPLOG_SYNC_INFO("map:", testMap);
    captured = getCapturedOutput(coutCollectorId);
    checkPattern(captured, "map: { 1 : Example | 2 : values | 3 : of | 4 : the | 5 : map }");

    // For logger message sending
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::filesystem::remove_all(testDirpath);
}