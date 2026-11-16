#include <gtest/gtest.h>

#include <Components/Encryption/FileGlue.h>
#include "gluefilemetadata.cpp"

#include <Components/Filework/TemporaryFile.h>

#include <filesystem>
#include <fstream>


struct SaveTestCase {
    uint64_t value;
    const char* description;
};
TEST(Components_Encryption, GLUE_endian) {
    const std::vector<SaveTestCase> test_cases = {
        { 0x0000000000000000ULL, "0 bits" },
        { 0xFFFFFFFFFFFFFFFFULL, "All 1" },

        { 0x00000000000000FFULL, "1st byte max" },
        { 0x0000000000000100ULL, "2nd byte max" },
        { 0x000000000000FFFFULL, "3rd byte max" },
        { 0x0000000000010000ULL, "3rd byte max +" },
        { 0x00000000FFFFFFFFULL, "4 byte max" },
        { 0x0000000100000000ULL, "5 byte min" },

        { 0x0102030405060708ULL, "Ordered bytes" },
        { 0xAA55AA55AA55AA55ULL, "Subordered bytes" },
        { 0x55AA55AA55AA55AAULL, "Inverted subordered bytes" },
        { 0xF0F0F0F0F0F0F0F0ULL, "Nibbles" },

        { 0x8000000000000000ULL, "Sign byte" },
        { 0x0000000000000001ULL, "Lowest sense byte" },
        { 0x7FFFFFFFFFFFFFFFULL, "int64_t max" }
    };

    for (const auto& [testValue, testDescription] : test_cases) {
        auto converted = Encryption::GlueFileMetadata::serializeValue(testValue);
        auto deconverted = Encryption::GlueFileMetadata::deserializeValue(converted);
        ASSERT_EQ(testValue, deconverted) << "Failed serialization case:" << testDescription;
    }
}

TEST(Components_Encryption, GLUE_metadata) {
    Filework::TemporaryFile tmpf;
    const auto byteCount = 10000;
    tmpf.write(Encryption::generateSymbols(byteCount).c_str(), byteCount);
    tmpf.flush();

    auto metadata = Encryption::GlueFileMetadata::fromFile(std::string(tmpf.getFilename()));
    ASSERT_TRUE(metadata.isValid()) << Encryption::getEncryptionErrorText();
    ASSERT_EQ(tmpf.size(), metadata.fileSize);
    ASSERT_EQ(Encryption::sha256file(tmpf.getFilename()), metadata.fileHash);
}

TEST(Components_Encryption, GLUE_invalidFiles) {
    Filework::TemporaryFile tmpA;
    Filework::TemporaryFile tmpB;

    ASSERT_FALSE(Encryption::glueFiles(std::string(tmpA.getFilename()),
                                       std::string(tmpB.getFilename()),
                                       "test.bin"));

    tmpB << "AAAAAAAAAAAAAA";
    tmpB.flush();
    ASSERT_FALSE(Encryption::glueFiles(std::string(tmpA.getFilename()),
                                       std::string(tmpB.getFilename()),
                                       "test.bin"));

    tmpB.clearFileData();
    tmpA << "BBBBBBBBBBBBBBBBBBBBB";
    tmpA.flush();
    ASSERT_FALSE(Encryption::glueFiles(std::string(tmpA.getFilename()),
                                       std::string(tmpB.getFilename()),
                                       "test.bin"));
}

// ======================================================== //
void testFile(const uint64_t fileSizeA, const uint64_t fileSizeB) {
    const std::string testDataA = Encryption::generateBytes(fileSizeA);
    const std::string testDataB = Encryption::generateBytes(fileSizeB);
    const std::string resultFile {"./test_glued.bin"};
    const std::string fileA {"./test_A.txt"};
    const std::string fileB {"./test_B.txt"};

    if (std::filesystem::exists(resultFile)) {
        std::filesystem::remove(resultFile);
    }
    if (std::filesystem::exists(fileA)) {
        std::filesystem::remove(fileA);
    }
    if (std::filesystem::exists(fileB)) {
        std::filesystem::remove(fileB);
    }

    { // COPY OF UPPER TEST
        Filework::TemporaryFile tmpA;
        tmpA.write(testDataA.c_str(), testDataA.size());
        tmpA.flush();

        Filework::TemporaryFile tmpB;
        tmpB.write(testDataB.c_str(), testDataB.size());
        tmpB.flush();

        const auto testGluedData = testDataA + testDataB;
        const auto sumDataSize = testGluedData.size() + Encryption::GLUE_METADATA_SIZE * 2;

        ASSERT_TRUE(Encryption::glueFiles(std::string(tmpA.getFilename()),
                                          std::string(tmpB.getFilename()),
                                          resultFile)) << "Error text: " << Encryption::getEncryptionErrorText();
        ASSERT_EQ(sumDataSize, std::filesystem::file_size(resultFile));

        std::ifstream tmpRes(resultFile, std::ios_base::in | std::ios_base::binary);
        ASSERT_TRUE(tmpRes.is_open());

        // Check data written
        char buffer[sumDataSize];
        auto readBytes = tmpRes.readsome(buffer, sumDataSize);
        tmpRes.close();
        ASSERT_EQ(sumDataSize, readBytes);
        ASSERT_EQ(testGluedData, std::string(buffer + Encryption::GLUE_METADATA_SIZE * 2, readBytes - Encryption::GLUE_METADATA_SIZE * 2));
    }

    ASSERT_TRUE(Encryption::splitGluedFiles(resultFile, fileA, fileB)) << "Error text: " << Encryption::getEncryptionErrorText();

    {
        std::ifstream tmpRes(fileA, std::ios_base::in | std::ios_base::binary);
        ASSERT_TRUE(tmpRes.is_open());

        // Check data written
        char buffer[testDataA.size()];
        auto readBytes = tmpRes.readsome(buffer, testDataA.size());
        tmpRes.close();
        ASSERT_EQ(testDataA.size(), readBytes);
        ASSERT_EQ(testDataA, std::string(buffer, testDataA.size()));
        ASSERT_EQ(testDataA.size(), std::filesystem::file_size(fileA));
    }

    {
        std::ifstream tmpRes(fileB, std::ios_base::in | std::ios_base::binary);
        ASSERT_TRUE(tmpRes.is_open());

        // Check data written
        char buffer[testDataB.size()];
        auto readBytes = tmpRes.readsome(buffer, testDataB.size());
        tmpRes.close();
        ASSERT_EQ(testDataB.size(), readBytes);
        ASSERT_EQ(testDataB, std::string(buffer, testDataB.size()));
        ASSERT_EQ(testDataB.size(), std::filesystem::file_size(fileB));
    }
}


TEST(Components_Encryption, GLUE_splitFiles) {
    testFile(10, 10);
    testFile(500, 500);
    testFile(12345, 54321);
}
