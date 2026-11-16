#include <gtest/gtest.h>

#include <Components/Filework/ConfigParsing/IniParser.h>
#include <Components/Filework/Common.h>

#include "testini.cpp"

#include <filesystem>
#include <fstream>

using namespace Filework;

void checkValues(const IniFileParser& parser) {
    // TODO: Check values
}

TEST(Components_Filework, IniRead) {
    const std::string testfilePath {"test.txt"};
    std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter = {
        reinterpret_cast<bool*>(1),
        [&](bool*){
            try {
                std::filesystem::remove(testfilePath);
            } catch (...) {
                // ignore exceptions here
            }
        }
    }; // Deletes file

    {
        std::ofstream tmpFile(testfilePath);
        tmpFile << TEST_INI_DATA;
        tmpFile.flush();
    }

    IniFileParser parser;
    ASSERT_TRUE(parser.read(testfilePath, true)) << parser.getLastErrorText();
    checkValues(parser);
}

TEST(Components_Filework, IniWrite) {
    const std::string testfilePath {"test.txt"};
    {
        std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter = {
            reinterpret_cast<bool*>(1),
            [&](bool*){
                try {
                    std::filesystem::remove(testfilePath);
                } catch (...) {
                    // ignore exceptions here
                }
            }
        }; // Deletes file

        {
            std::ofstream tmpFile(testfilePath);
            tmpFile << TEST_INI_DATA;
            tmpFile.flush();
        }

        IniFileParser parser;
        ASSERT_TRUE(parser.read(testfilePath, true)) << parser.getLastErrorText();
        checkValues(parser);

        ASSERT_TRUE(parser.write("test_inifwrite.ini")) << parser.getLastErrorText();

        std::string tmpOutput;
        ASSERT_TRUE(Common::readFileData("test_inifwrite.ini", tmpOutput));
        std::filesystem::remove("test_inifwrite.ini");
        ASSERT_EQ(TEST_INI_EXPECTED_DATA, tmpOutput);
    }
}