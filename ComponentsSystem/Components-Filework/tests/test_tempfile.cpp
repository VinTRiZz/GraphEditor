#include <gtest/gtest.h>

#include <Components/Filework/TemporaryFile.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>

TEST(Components_Filework, TemporaryFileNoTarget) {
    const std::string TEST_TEXT {"Test_text"};
    Filework::TemporaryFile tmpFile;

    try {
        tmpFile << TEST_TEXT;
    } catch (...) {
        std::cerr << "Temporary file writing error" << std::endl;
        throw;
    }

    try {
        std::string fileContents;
        tmpFile.seekg(0, std::ios_base::beg);
        tmpFile >> fileContents;
        ASSERT_EQ(TEST_TEXT, fileContents);
    } catch (...) {
        std::cerr << "Temporary file reading error" << std::endl;
        throw;
    }

    try {
        tmpFile.clearFileData();
        ASSERT_TRUE(tmpFile.empty()) << "File not cleaned (size is " << tmpFile.size() << ")";
    } catch (...) {
        std::cerr << "Temporary file cleaning error" << std::endl;
        throw;
    }
}

TEST(Components_Filework, TemporaryFileRAII) {
    std::string fileName;
    {
        const std::string testfilePath {"tempfile_input.txt"};
        std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter = {
            reinterpret_cast<bool*>(1),
            [&](bool*){
                if (std::filesystem::exists(testfilePath)) {
                    std::filesystem::remove(testfilePath);
                }
            }
        }; // Deletes file

        {
            std::ofstream testfile(testfilePath);
            testfile << "Empty text";
            testfile.flush();
        }

        Filework::TemporaryFile tmpFile(testfilePath);
        fileName = tmpFile.getFilename();
    }
    ASSERT_FALSE(std::filesystem::exists(fileName)) << "RAII broken";
}

TEST(Components_Filework, TemporaryFileTarget) {
    std::string tmpFileName;
    {
        const std::string TEST_TEXT {"Test_text"};
        const std::string TEST_TEXT_REPL {"Replaced_data"};

        const std::string testfilePath {"tempfile_input.txt"};
        if (std::filesystem::exists(testfilePath)) {
            std::filesystem::remove(testfilePath);
        }

        {
            std::ofstream testfile(testfilePath);
            testfile << TEST_TEXT;
            testfile.flush();
        }

        // Check temporary file writing
        Filework::TemporaryFile tmpFile(testfilePath);
        tmpFileName = std::string(tmpFile.getFilename());
        try {
            tmpFile.seekg(0, std::ios_base::end);
            tmpFile << TEST_TEXT_REPL;
            tmpFile.flush();
        } catch (const std::exception& ex) {
            std::cerr << "Failed to write data to temporary file" << std::endl;
            throw;
        }

        // Check that data have even been written
        try {
            std::string fileContents;
            tmpFile.seekg(0, std::ios_base::beg);
            tmpFile >> fileContents;
            ASSERT_EQ(std::string(TEST_TEXT) + TEST_TEXT_REPL, fileContents);
        } catch (const std::exception& ex) {
            std::cerr << "Failed to read data 1 from temporary file" << std::endl;
            throw;
        }

        // Try save
        try {
            tmpFile.accept();
        } catch (const std::exception& ex) {
            std::cerr << "Failed to accept temporary file" << std::endl;
            throw;
        }

        // Reopen file, check if data is correct
        ASSERT_TRUE(tmpFile.setTargetFile(testfilePath)) << "Failed to set target file";
        try {
            std::string fileContents;
            tmpFile.seekg(0, std::ios_base::beg);
            tmpFile >> fileContents;
            ASSERT_EQ(std::string(TEST_TEXT) + TEST_TEXT_REPL, fileContents);
        } catch (const std::exception& ex) {
            std::cerr << "Failed to read data 2 from temporary file" << std::endl;
            throw;
        }

        // Here's must be all good with used methods
        tmpFile.clearFileData();
        tmpFile << TEST_TEXT;
        tmpFile.flush();
        tmpFile.accept();

        {
            std::ifstream testfile(testfilePath);
            std::string fileContents;
            testfile >> fileContents;
            ASSERT_EQ(TEST_TEXT, fileContents);
        }

        if (std::filesystem::exists(testfilePath)) {
            std::filesystem::remove(testfilePath);
        }
    }
    ASSERT_FALSE(std::filesystem::exists(tmpFileName)) << "RAII broken";
}

namespace {

class Components_FileworkTemporaryFile : public ::testing::Test {
protected:
    const std::string testDir = "temporaryfile_test_dir";

    void SetUp() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::string path(const std::string& name) const {
        return (std::filesystem::path(testDir) / name).string();
    }

    void createFile(const std::string& name, const std::string& content) const {
        std::ofstream file(path(name), std::ios_base::binary | std::ios_base::trunc);
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
    }

    static std::string readBytes(const std::string& filePath) {
        std::ifstream file(filePath, std::ios_base::binary | std::ios_base::in);
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    // Payload that covers every byte value, including NUL and whitespace
    static std::string makeBinaryPayload(std::size_t size) {
        std::string content(size, '\0');
        for (std::size_t i = 0; i < size; ++i) {
            content[i] = static_cast<char>(i % 256);
        }
        return content;
    }
};

} // namespace

class Components_FileworkTemporaryFileSizes
    : public Components_FileworkTemporaryFile
    , public ::testing::WithParamInterface<std::size_t> {
};

// ---------------- STL compatibility ----------------

TEST(Components_FileworkTemporaryFileTraits, IsNotCopyableAndNotMovable) {
    static_assert(std::is_default_constructible_v<Filework::TemporaryFile>);
    static_assert(std::is_destructible_v<Filework::TemporaryFile>);

    static_assert(!std::is_copy_constructible_v<Filework::TemporaryFile>);
    static_assert(!std::is_copy_assignable_v<Filework::TemporaryFile>);
    static_assert(!std::is_move_constructible_v<Filework::TemporaryFile>);
    static_assert(!std::is_move_assignable_v<Filework::TemporaryFile>);
}

TEST_F(Components_FileworkTemporaryFile, StreamOperatorsSupportStandardTypes) {
    Filework::TemporaryFile tmpFile;
    tmpFile << 42 << ' ' << 2.5 << ' ' << "text";

    int intValue {0};
    double doubleValue {0.0};
    std::string stringValue;

    tmpFile.seekg(0, std::ios_base::beg);
    tmpFile >> intValue >> doubleValue >> stringValue;

    ASSERT_EQ(intValue, 42);
    ASSERT_DOUBLE_EQ(doubleValue, 2.5);
    ASSERT_EQ(stringValue, "text");
}

// ---------------- Writing and reading back everything ----------------

TEST_F(Components_FileworkTemporaryFile, WriteStoresEveryWrittenByte) {
    const std::string payload = makeBinaryPayload(100000);

    Filework::TemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.empty());

    tmpFile.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    tmpFile.flush();

    ASSERT_EQ(tmpFile.size(), payload.size());
    ASSERT_FALSE(tmpFile.empty());
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), payload);
}

TEST_F(Components_FileworkTemporaryFile, WriteCanBeChained) {
    Filework::TemporaryFile tmpFile;

    tmpFile.write("abc", 3).write("def", 3);
    tmpFile.flush();

    ASSERT_EQ(tmpFile.size(), 6u);
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), "abcdef");
}

TEST_F(Components_FileworkTemporaryFile, ReadsomeReturnsWrittenData) {
    const std::string payload = makeBinaryPayload(64);

    Filework::TemporaryFile tmpFile;
    tmpFile.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    tmpFile.flush();
    tmpFile.seekg(0, std::ios_base::beg);

    std::string output(payload.size(), '\0');
    std::streamsize totalRead {0};
    while (totalRead < static_cast<std::streamsize>(payload.size())) {
        const auto read = tmpFile.readsome(
            output.data() + totalRead,
            static_cast<std::streamsize>(payload.size()) - totalRead);
        if (read <= 0) {
            break;
        }
        totalRead += read;
    }

    ASSERT_EQ(totalRead, static_cast<std::streamsize>(payload.size()));
    ASSERT_EQ(output, payload);
}

TEST_F(Components_FileworkTemporaryFile, AcceptWritesEveryWrittenByteToTarget) {
    const std::string payload = makeBinaryPayload(70000);
    const std::string target = path("target.bin");

    Filework::TemporaryFile tmpFile(target);
    tmpFile.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    tmpFile.flush();
    tmpFile.accept();

    ASSERT_TRUE(std::filesystem::exists(target));
    ASSERT_EQ(std::filesystem::file_size(target), payload.size());
    ASSERT_EQ(readBytes(target), payload);
}

TEST_F(Components_FileworkTemporaryFile, AcceptCreatesMissingTarget) {
    const std::string target = path("new_target.bin");

    Filework::TemporaryFile tmpFile(target);
    tmpFile << "created";
    tmpFile.flush();
    ASSERT_FALSE(std::filesystem::exists(target));

    tmpFile.accept();

    ASSERT_TRUE(std::filesystem::exists(target));
    ASSERT_EQ(readBytes(target), "created");
}

TEST_F(Components_FileworkTemporaryFile, AcceptReopensEmptyTemporaryFile) {
    const std::string target = path("target.bin");
    createFile("target.bin", "old content");

    Filework::TemporaryFile tmpFile(target);
    tmpFile.clearFileData();
    tmpFile << "new content";
    tmpFile.flush();
    tmpFile.accept();

    ASSERT_EQ(readBytes(target), "new content");
    ASSERT_TRUE(tmpFile.empty());
    ASSERT_TRUE(std::filesystem::exists(std::string(tmpFile.getFilename())));
}

TEST_F(Components_FileworkTemporaryFile, AcceptWithoutTargetIsNoOp) {
    Filework::TemporaryFile tmpFile;
    tmpFile << "kept";
    tmpFile.flush();

    const std::string tmpPath {tmpFile.getFilename()};
    tmpFile.accept();

    ASSERT_TRUE(tmpFile.getTargetFilename().empty());
    ASSERT_EQ(readBytes(tmpPath), "kept");
    ASSERT_EQ(tmpFile.size(), 4u);
}

// ---------------- Target handling ----------------

TEST_F(Components_FileworkTemporaryFile, TargetFilenamesAreReported) {
    const std::string target = path("target.bin");

    Filework::TemporaryFile withoutTarget;
    ASSERT_TRUE(withoutTarget.getTargetFilename().empty());
    ASSERT_FALSE(std::string(withoutTarget.getFilename()).empty());

    Filework::TemporaryFile withTarget(target);
    ASSERT_EQ(std::string(withTarget.getTargetFilename()), target);
}

TEST_F(Components_FileworkTemporaryFile, CopyContentsIsEnabledByDefault) {
    const std::string original = "original contents";
    createFile("target.bin", original);

    Filework::TemporaryFile tmpFile(path("target.bin"));

    ASSERT_EQ(tmpFile.size(), original.size());
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), original);
}

TEST_F(Components_FileworkTemporaryFile, CopyContentsCanBeDisabled) {
    createFile("target.bin", "original contents");

    Filework::TemporaryFile tmpFile(path("target.bin"), false);

    ASSERT_TRUE(tmpFile.empty());
    ASSERT_EQ(std::string(tmpFile.getTargetFilename()), path("target.bin"));
}

TEST_F(Components_FileworkTemporaryFile, SetTargetFileResetsTargetOnEmptyPath) {
    createFile("target.bin", "data");

    Filework::TemporaryFile tmpFile(path("target.bin"));
    tmpFile << "more";
    tmpFile.flush();

    ASSERT_TRUE(tmpFile.setTargetFile(""));
    ASSERT_TRUE(tmpFile.getTargetFilename().empty());
    ASSERT_TRUE(tmpFile.empty());
}

TEST_F(Components_FileworkTemporaryFile, SetTargetFileRejectsDirectory) {
    std::filesystem::create_directories(path("subdir"));

    Filework::TemporaryFile tmpFile;
    EXPECT_THROW(tmpFile.setTargetFile(path("subdir")), std::runtime_error);
}

TEST_F(Components_FileworkTemporaryFile, SetTargetFileRejectsReadOnlyFile) {
    createFile("readonly.bin", "data");
    std::filesystem::permissions(
        path("readonly.bin"),
        std::filesystem::perms::owner_read,
        std::filesystem::perm_options::replace);

    Filework::TemporaryFile tmpFile;
    EXPECT_THROW(tmpFile.setTargetFile(path("readonly.bin")), std::runtime_error);

    std::filesystem::permissions(
        path("readonly.bin"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

// ---------------- Cleaning, size and secure mode ----------------

TEST_F(Components_FileworkTemporaryFile, ClearFileDataWipesAndAllowsReuse) {
    Filework::TemporaryFile tmpFile;
    tmpFile << "first";
    tmpFile.flush();
    ASSERT_EQ(tmpFile.size(), 5u);

    tmpFile.clearFileData();
    ASSERT_TRUE(tmpFile.empty());
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), "");

    tmpFile << "second";
    tmpFile.flush();
    ASSERT_EQ(tmpFile.size(), 6u);
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), "second");
}

TEST_F(Components_FileworkTemporaryFile, SecureModeCanBeEnabledAndFileStillWorks) {
    const std::string target = path("target.bin");

    Filework::TemporaryFile tmpFile(target);
    EXPECT_NO_THROW(tmpFile.setSecureMode(true));
    EXPECT_NO_THROW(tmpFile.setSecureMode(false));
    EXPECT_NO_THROW(tmpFile.setSecureMode(true));

    tmpFile << "secure data";
    tmpFile.flush();
    ASSERT_EQ(tmpFile.size(), 11u);

    tmpFile.accept();
    ASSERT_EQ(readBytes(target), "secure data");
}

// ---------------- Stream state ----------------

TEST_F(Components_FileworkTemporaryFile, SeekgAndTellgTrackPosition) {
    Filework::TemporaryFile tmpFile;
    tmpFile << "abcdef";
    tmpFile.flush();

    ASSERT_TRUE(tmpFile.seekg(0, std::ios_base::beg));
    ASSERT_EQ(tmpFile.tellg(), std::size_t{0});

    ASSERT_TRUE(tmpFile.seekg(3, std::ios_base::beg));
    ASSERT_EQ(tmpFile.tellg(), std::size_t{3});

    char character {'\0'};
    tmpFile >> character;
    ASSERT_EQ(character, 'd');
}

TEST_F(Components_FileworkTemporaryFile, EofIsSetAfterReadingToEnd) {
    Filework::TemporaryFile tmpFile;
    tmpFile << "abc";
    tmpFile.flush();
    tmpFile.seekg(0, std::ios_base::beg);
    ASSERT_FALSE(tmpFile.eof());

    std::string contents;
    tmpFile >> contents;

    ASSERT_EQ(contents, "abc");
    ASSERT_TRUE(tmpFile.eof());
}

TEST_F(Components_FileworkTemporaryFile, ClearBadbitRecoversFromFailedRead) {
    Filework::TemporaryFile tmpFile;
    tmpFile << "not_a_number";
    tmpFile.flush();
    tmpFile.seekg(0, std::ios_base::beg);
    ASSERT_FALSE(tmpFile.fail());
    ASSERT_FALSE(tmpFile.bad());

    int value {0};
    tmpFile >> value;

    ASSERT_TRUE(tmpFile.fail());
    ASSERT_FALSE(tmpFile.bad());

    tmpFile.clearBadbit();

    ASSERT_FALSE(tmpFile.fail());
    ASSERT_FALSE(tmpFile.bad());
}

TEST_F(Components_FileworkTemporaryFile, TemporaryFileIsRemovedOnDestruction) {
    std::string tmpPath;
    {
        Filework::TemporaryFile tmpFile;
        tmpPath = std::string(tmpFile.getFilename());
        ASSERT_TRUE(std::filesystem::exists(tmpPath));
    }
    ASSERT_FALSE(std::filesystem::exists(tmpPath));
}

// ---------------- Small / medium / large files ----------------
// Sizes: 10 bytes (small), 3000 bytes (medium), 90000 bytes (large)

TEST_P(Components_FileworkTemporaryFileSizes, WriteAndReadBackExactSize) {
    const std::size_t size = GetParam();
    const std::string payload = makeBinaryPayload(size);

    Filework::TemporaryFile tmpFile;
    ASSERT_TRUE(tmpFile.empty());

    tmpFile.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    tmpFile.flush();

    ASSERT_EQ(tmpFile.size(), size);
    ASSERT_EQ(readBytes(std::string(tmpFile.getFilename())), payload);
}

TEST_P(Components_FileworkTemporaryFileSizes, AcceptWritesExactSizeAndContent) {
    const std::size_t size = GetParam();
    const std::string payload = makeBinaryPayload(size);
    const std::string target = path("sized_target.bin");

    Filework::TemporaryFile tmpFile(target);
    tmpFile.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    tmpFile.flush();
    tmpFile.accept();

    ASSERT_EQ(std::filesystem::file_size(target), size);
    ASSERT_EQ(readBytes(target), payload);
}

INSTANTIATE_TEST_SUITE_P(
    SmallMediumLarge,
    Components_FileworkTemporaryFileSizes,
    ::testing::Values(std::size_t{10}, std::size_t{3000}, std::size_t{90000}),
    [](const ::testing::TestParamInfo<Components_FileworkTemporaryFileSizes::ParamType>& info) {
        return std::to_string(info.param) + "Bytes";
    });
