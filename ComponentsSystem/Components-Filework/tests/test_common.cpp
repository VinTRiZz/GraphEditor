#include <gtest/gtest.h>

#include <Components/Filework/Common.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class Components_FileworkCommon : public ::testing::Test {
protected:
    const std::string testDir = "filework_test_dir";

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
        return std::filesystem::path(testDir) / name;
    }

    void createFile(const std::string& name, const std::string& content = "") {
        std::ofstream f(path(name), std::ios_base::binary | std::ios_base::trunc);
        f << content;
    }

    void createDir(const std::string& name) {
        std::filesystem::create_directories(path(name));
    }

    static std::string makeContent(std::size_t size) {
        std::string content;
        content.reserve(size);
        for (std::size_t i = 0; i < size; ++i) {
            content.push_back(static_cast<char>('a' + (i % 26)));
        }
        return content;
    }
};

namespace {

std::string toUpper(std::string_view input) {
    std::string result(input);
    for (char& character : result) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}

std::string replaceSymbols(std::string_view input) {
    std::string result(input);
    for (char& character : result) {
        switch (character) {
            case 'a': character = 'A'; break;
            case 'b': character = 'B'; break;
            case 'c': character = 'C'; break;
            case 'x': character = 'X'; break;
            case 'y': character = 'Y'; break;
            case 'z': character = 'Z'; break;
            case '1': character = '0'; break;
            case '-': character = '_'; break;
            case ' ': character = '.'; break;
            default: break;
        }
    }
    return result;
}

class Components_FileworkCommonSizes
    : public Components_FileworkCommon
    , public ::testing::WithParamInterface<std::size_t> {
};

} // namespace

// ---------------- Filework::Common::getBufferSize ----------------

TEST_F(Components_FileworkCommon, GetBufferSize_Positive) {
    ASSERT_GT(Filework::Common::getBufferSize(), 0);
}

// ---------------- Filework::Common::objectExist / Filework::Common::contains ----------------

TEST_F(Components_FileworkCommon, ObjectExist_FileAndDirAndMissing) {
    createFile("a.txt");
    createDir("subdir");

    ASSERT_TRUE(Filework::Common::objectExist(path("a.txt")));
    ASSERT_TRUE(Filework::Common::objectExist(path("subdir")));
    ASSERT_FALSE(Filework::Common::objectExist(path("missing.txt")));
}

TEST_F(Components_FileworkCommon, Contains_PresentAndAbsent) {
    createFile("a.txt");
    ASSERT_TRUE(Filework::Common::contains(testDir, "a.txt"));
    ASSERT_FALSE(Filework::Common::contains(testDir, "b.txt"));
}

TEST_F(Components_FileworkCommon, Contains_NotADirectory) {
    createFile("a.txt");
    ASSERT_FALSE(Filework::Common::contains(path("a.txt"), "a.txt"));
    ASSERT_FALSE(Filework::Common::contains(path("missing_dir"), "a.txt"));
}

// ---------------- Filework::Common::isReadable / isWritable ----------------

TEST_F(Components_FileworkCommon, IsReadable_FileAndDir) {
    createFile("a.txt", "data");
    createDir("subdir");

    ASSERT_TRUE(Filework::Common::isReadable(path("a.txt")));
    ASSERT_TRUE(Filework::Common::isReadable(path("subdir")));
}

TEST_F(Components_FileworkCommon, IsReadable_WriteOnlyFile) {
    createFile("a.txt", "data");
    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace);

    ASSERT_FALSE(Filework::Common::isReadable(path("a.txt")));

    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

TEST_F(Components_FileworkCommon, IsWritable_FileAndDir) {
    createFile("a.txt", "data");
    createDir("subdir");

    ASSERT_TRUE(Filework::Common::isWritable(path("a.txt")));
    ASSERT_TRUE(Filework::Common::isWritable(path("subdir")));
}

TEST_F(Components_FileworkCommon, IsWritable_ReadOnlyFile) {
    createFile("a.txt", "data");
    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_read,
        std::filesystem::perm_options::replace);

    ASSERT_TRUE(Filework::Common::isReadable(path("a.txt")));
    ASSERT_FALSE(Filework::Common::isWritable(path("a.txt")));

    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

// ---------------- Filework::Common::isAccessable ----------------

TEST_F(Components_FileworkCommon, IsAccessable_ExistingFileAndDir) {
    createFile("a.txt", "data");
    createDir("subdir");

    auto fileErr = Filework::Common::isAccessable(path("a.txt"));
    ASSERT_TRUE(fileErr) << fileErr.getDetailText();

    auto dirErr = Filework::Common::isAccessable(path("subdir"));
    ASSERT_TRUE(dirErr) << dirErr.getDetailText();
}

TEST_F(Components_FileworkCommon, IsAccessable_Missing) {
    auto err = Filework::Common::isAccessable(path("missing.txt"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

// ---------------- Counts ----------------

TEST_F(Components_FileworkCommon, ObjectCount_MixedContent) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createFile("d.txt");
    createFile("e.txt");
    createDir("dir1");
    createDir("dir2");
    createDir("dir3");

    // 5 files, 3 dirs. In count error must be 10, 6, or 4
    ASSERT_EQ(static_cast<int>(Filework::Common::objectCount(testDir)), 8);
}

TEST_F(Components_FileworkCommon, ObjectCount_EmptyDir) {
    ASSERT_EQ(static_cast<int>(Filework::Common::objectCount(testDir)), 0);
}

TEST_F(Components_FileworkCommon, Counts_NonexistentDir) {
    ASSERT_EQ(static_cast<int>(Filework::Common::objectCount(path("missing"))), 0);
    ASSERT_EQ(static_cast<int>(Filework::Common::fileCount(path("missing"))), 0);
    ASSERT_EQ(static_cast<int>(Filework::Common::dirCount(path("missing"))), 0);
}

TEST_F(Components_FileworkCommon, FilesCount_OnlyRegularFiles) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createFile("d.txt");
    createFile("e.txt");
    createDir("subdir1");
    createDir("subdir2");
    ASSERT_EQ(static_cast<int>(Filework::Common::fileCount(testDir)), 5);
}

TEST_F(Components_FileworkCommon, DirsCount_OnlyDirs) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createDir("dir1");
    createDir("dir2");
    createDir("dir3");
    createDir("dir4");
    createDir("dir5");
    ASSERT_EQ(static_cast<int>(Filework::Common::dirCount(testDir)), 5);
}

// ---------------- getContentNames / getContentPaths ----------------

TEST_F(Components_FileworkCommon, GetContentNames_AllNames) {
    createFile("a.txt");
    createFile("b.log");
    createDir("subdir");

    auto names = Filework::Common::getContentNames(testDir);
    ASSERT_EQ(names.size(), 3);
    ASSERT_EQ(names[0], "a.txt");
    ASSERT_EQ(names[1], "b.log");
    ASSERT_EQ(names[2], "subdir");
}

TEST_F(Components_FileworkCommon, GetContentNames_WithRegexpFilter) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.log");

    auto names = Filework::Common::getContentNames(testDir, ".*\\.txt");
    ASSERT_EQ(names.size(), 2);
    for (const auto& n : names) {
        ASSERT_NE(n.find(".txt"), std::string::npos);
    }
}

TEST_F(Components_FileworkCommon, GetContentNames_NonexistentDir) {
    auto names = Filework::Common::getContentNames(path("missing"));
    ASSERT_TRUE(names.empty());
}

TEST_F(Components_FileworkCommon, GetContentPaths_AllPaths) {
    createFile("a.txt");
    createFile("b.log");
    createDir("subdir");

    auto paths = Filework::Common::getContentPaths(testDir);
    ASSERT_EQ(paths.size(), 3u);
    for (const auto& p : paths) {
        ASSERT_TRUE(std::filesystem::exists(p));
        ASSERT_EQ(p.rfind(testDir, 0), 0); // префикс каталога
    }
}

TEST_F(Components_FileworkCommon, GetContentPaths_SortedAndNonexistentDir) {
    createFile("c.txt");
    createFile("a.txt");
    createDir("b_dir");

    auto paths = Filework::Common::getContentPaths(testDir);
    ASSERT_EQ(paths.size(), 3u);
    ASSERT_TRUE(std::is_sorted(paths.begin(), paths.end()));
    ASSERT_EQ(paths[0], path("a.txt"));
    ASSERT_EQ(paths[1], path("b_dir"));
    ASSERT_EQ(paths[2], path("c.txt"));

    ASSERT_TRUE(Filework::Common::getContentPaths(path("missing")).empty());
}

// ---------------- readFileData / saveFileData / appendFileData ----------------

TEST_F(Components_FileworkCommon, ReadData) {
    const std::string content = "hello world";
    createFile("test.txt", content);

    std::string out;
    auto err = Filework::Common::readFileData(path("test.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, ReadData_EmptyFile) {
    createFile("empty.txt");

    std::string out = "stale";
    auto err = Filework::Common::readFileData(path("empty.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_TRUE(out.empty());
}

TEST_F(Components_FileworkCommon, ReadData_DirectoryReportsFailure) {
    createDir("subdir");
    std::string out;
    auto err = Filework::Common::readFileData(path("subdir"), out);
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkCommon, ReadData_NotReadableReportsFailure) {
    createFile("a.txt", "data");
    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace);

    std::string out;
    auto err = Filework::Common::readFileData(path("a.txt"), out);
    ASSERT_FALSE(err) << err.getDetailText();

    std::filesystem::permissions(
        path("a.txt"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

TEST_F(Components_FileworkCommon, SaveData) {
    const std::string content = "hello world";
    auto err = Filework::Common::saveFileData(path("f.txt"), content);
    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, SaveData_EmptyContent) {
    auto err = Filework::Common::saveFileData(path("f.txt"), "");
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_TRUE(std::filesystem::exists(path("f.txt")));
    ASSERT_EQ(std::filesystem::file_size(path("f.txt")), 0u);
}

TEST_F(Components_FileworkCommon, SaveData_LargerThanBuffer) {
    const auto content = makeContent(static_cast<std::size_t>(Filework::Common::getBufferSize()) * 2 + 5);
    auto err = Filework::Common::saveFileData(path("big.txt"), content);
    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("big.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, SaveData_DirectoryReportsFailure) {
    createDir("subdir");
    auto err = Filework::Common::saveFileData(path("subdir"), "data");
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkCommon, SaveData_ReadOnlyFileReportsFailure) {
    createFile("f.txt", "data");
    std::filesystem::permissions(
        path("f.txt"),
        std::filesystem::perms::owner_read,
        std::filesystem::perm_options::replace);

    auto err = Filework::Common::saveFileData(path("f.txt"), "new data");
    ASSERT_FALSE(err) << err.getDetailText();

    std::filesystem::permissions(
        path("f.txt"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

TEST_F(Components_FileworkCommon, ReadNonexistentFile_ReportsFailure) {
    std::string out;
    auto err = Filework::Common::readFileData(path("missing.txt"), out);
    ASSERT_FALSE(err) << err.getDetailText();
}

TEST_F(Components_FileworkCommon, ReplaceFileData_Overwrites) {
    auto err = Filework::Common::saveFileData(path("f.txt"), "original");
    ASSERT_TRUE(err) << err.getDetailText();
    std::string out;
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "original");

    // Replacement check
    out.clear();
    err = Filework::Common::saveFileData(path("f.txt"), "replaced");
    ASSERT_TRUE(err) << err.getDetailText();
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "replaced");
}

TEST_F(Components_FileworkCommon, AppendFileData_Appends) {
    auto err = Filework::Common::saveFileData(path("f.txt"), "first");
    ASSERT_TRUE(err) << err.getDetailText();
    std::string out;
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "first");

    out.clear();
    err = Filework::Common::appendFileData(path("f.txt"), " second");
    ASSERT_TRUE(err) << err.getDetailText();
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "first second");
}

TEST_F(Components_FileworkCommon, AppendFileData_CreatesMissingFile) {
    auto err = Filework::Common::appendFileData(path("new.txt"), "created");
    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("new.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "created");
}

TEST_F(Components_FileworkCommon, AppendFileData_DirectoryReportsFailure) {
    createDir("subdir");
    auto err = Filework::Common::appendFileData(path("subdir"), "data");
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

// ---------------- streamReadFileData ----------------

TEST_F(Components_FileworkCommon, StreamReadFileData_ReadsAllInChunks) {
    const auto content = makeContent(static_cast<std::size_t>(Filework::Common::getBufferSize()) * 2 + 17);
    createFile("big.txt", content);

    std::string out;
    std::size_t callCount = 0;
    auto err = Filework::Common::streamReadFileData(
        path("big.txt"),
        [&out, &callCount](const std::string_view& data, bool) -> bool {
            ++callCount;
            out.append(data.data(), data.size());
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
    ASSERT_GE(callCount, 3u);
}

TEST_F(Components_FileworkCommon, StreamReadFileData_EmptyFile) {
    createFile("empty.txt");

    std::string out;
    std::size_t callCount = 0;
    auto err = Filework::Common::streamReadFileData(
        path("empty.txt"),
        [&out, &callCount](const std::string_view& data, bool) -> bool {
            ++callCount;
            out.append(data.data(), data.size());
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_TRUE(out.empty());
    ASSERT_EQ(callCount, 0u);
}

TEST_F(Components_FileworkCommon, StreamReadFileData_CallbackStopsReading) {
    const auto bufferSize = static_cast<std::size_t>(Filework::Common::getBufferSize());
    const auto content = makeContent(bufferSize * 2 + 5);
    createFile("big.txt", content);

    std::string out;
    auto err = Filework::Common::streamReadFileData(
        path("big.txt"),
        [&out](const std::string_view& data, bool) -> bool {
            out.append(data.data(), data.size());
            return false;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out.size(), bufferSize);
    ASSERT_EQ(out, content.substr(0, bufferSize));
}

TEST_F(Components_FileworkCommon, StreamReadFileData_DirectoryReportsFailure) {
    createDir("subdir");
    auto err = Filework::Common::streamReadFileData(
        path("subdir"),
        [](const std::string_view&, bool) -> bool { return true; });
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkCommon, StreamReadFileData_MissingReportsFailure) {
    auto err = Filework::Common::streamReadFileData(
        path("missing.txt"),
        [](const std::string_view&, bool) -> bool { return true; });
    ASSERT_FALSE(err) << err.getDetailText();
}

// ---------------- streamWriteFileData ----------------

TEST_F(Components_FileworkCommon, StreamWriteFileData_WritesAll) {
    const std::string content = "hello stream";
    std::size_t written = 0;
    auto err = Filework::Common::streamWriteFileData(
        path("f.txt"),
        [&content, &written](char* buffer, std::streamsize& writeCount) -> bool {
            const auto left = std::min<std::size_t>(
                static_cast<std::size_t>(Filework::Common::getBufferSize()),
                content.size() - written);
            std::copy_n(content.data() + written, left, buffer);
            written += left;
            writeCount = static_cast<std::streamsize>(left);
            return left > 0;
        });

    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, StreamWriteFileData_LargerThanBuffer) {
    const auto content = makeContent(static_cast<std::size_t>(Filework::Common::getBufferSize()) * 2 + 9);
    std::size_t written = 0;
    auto err = Filework::Common::streamWriteFileData(
        path("big.txt"),
        [&content, &written](char* buffer, std::streamsize& writeCount) -> bool {
            const auto left = std::min<std::size_t>(
                static_cast<std::size_t>(Filework::Common::getBufferSize()),
                content.size() - written);
            std::copy_n(content.data() + written, left, buffer);
            written += left;
            writeCount = static_cast<std::streamsize>(left);
            return left > 0;
        });

    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("big.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, StreamWriteFileData_ReplacesExistingContent) {
    createFile("f.txt", "old content");

    const std::string content = "new";
    std::size_t written = 0;
    auto err = Filework::Common::streamWriteFileData(
        path("f.txt"),
        [&content, &written](char* buffer, std::streamsize& writeCount) -> bool {
            const auto left = content.size() - written;
            std::copy_n(content.data() + written, left, buffer);
            written += left;
            writeCount = static_cast<std::streamsize>(left);
            return left > 0;
        });

    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("f.txt"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, StreamWriteFileData_DirectoryReportsFailure) {
    createDir("subdir");
    auto err = Filework::Common::streamWriteFileData(
        path("subdir"),
        [](char*, std::streamsize& writeCount) -> bool {
            writeCount = 0;
            return false;
        });
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

// ---------------- streamTransformFileData ----------------

TEST_F(Components_FileworkCommon, StreamTransformFileData_ReplacesDifferentSymbols) {
    const std::string original = "abcxyz-123 xyz ABC";
    const std::string expected = "ABCXYZ_023.XYZ.ABC";
    createFile("symbols.bin", original);

    auto err = Filework::Common::streamTransformFileData(
        path("symbols.bin"),
        [](const std::string_view& fileData, std::string& replaceData, bool) -> bool {
            replaceData = replaceSymbols(fileData);
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("symbols.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, expected);
    ASSERT_EQ(out.find("abc"), std::string::npos);
    ASSERT_EQ(out.find("xyz"), std::string::npos);
    ASSERT_EQ(out.find("-123"), std::string::npos);
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_ReplacesWholeFileContent) {
    createFile("target.bin", "old data that must be replaced completely");

    auto err = Filework::Common::streamTransformFileData(
        path("target.bin"),
        [](const std::string_view& fileData, std::string& replaceData, bool) -> bool {
            replaceData = toUpper(fileData);
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();

    std::string out;
    err = Filework::Common::readFileData(path("target.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, "OLD DATA THAT MUST BE REPLACED COMPLETELY");
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_MissingFileReportsFailure) {
    auto err = Filework::Common::streamTransformFileData(
        path("missing.bin"),
        [](const std::string_view&, std::string& replaceData, bool) -> bool {
            replaceData.clear();
            return true;
        });
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_DirectoryReportsFailure) {
    createDir("subdir");
    auto err = Filework::Common::streamTransformFileData(
        path("subdir"),
        [](const std::string_view&, std::string& replaceData, bool) -> bool {
            replaceData.clear();
            return true;
        });
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_ReadOnlyFileReportsFailure) {
    createFile("readonly.bin", "data");
    std::filesystem::permissions(
        path("readonly.bin"),
        std::filesystem::perms::owner_read,
        std::filesystem::perm_options::replace);

    auto err = Filework::Common::streamTransformFileData(
        path("readonly.bin"),
        [](const std::string_view&, std::string& replaceData, bool) -> bool {
            replaceData.clear();
            return true;
        });
    ASSERT_FALSE(err) << err.getDetailText();

    std::filesystem::permissions(
        path("readonly.bin"),
        std::filesystem::perms::owner_all,
        std::filesystem::perm_options::replace);
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_OversizedReplacementReportsFailure) {
    const std::string original = "unchanged data";
    createFile("target.bin", original);

    auto err = Filework::Common::streamTransformFileData(
        path("target.bin"),
        [](const std::string_view&, std::string& replaceData, bool) -> bool {
            replaceData.assign(static_cast<std::size_t>(Filework::Common::getBufferSize()) + 1, 'x');
            return true;
        });
    ASSERT_FALSE(err) << err.getDetailText();

    // Failed transformation must not touch the target file
    std::string out;
    auto readErr = Filework::Common::readFileData(path("target.bin"), out);
    ASSERT_TRUE(readErr) << readErr.getDetailText();
    ASSERT_EQ(out, original);
}

TEST_F(Components_FileworkCommon, StreamTransformFileData_CallbackStopsTransformation) {
    const std::string original = makeContent(90000);
    createFile("target.bin", original);

    bool firstChunkProcessed = false;
    std::string processed;
    auto err = Filework::Common::streamTransformFileData(
        path("target.bin"),
        [&firstChunkProcessed, &processed](const std::string_view& fileData, std::string& replaceData, bool) -> bool {
            if (firstChunkProcessed) {
                return false;
            }
            firstChunkProcessed = true;
            processed = toUpper(fileData);
            replaceData = processed;
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_FALSE(processed.empty());
    ASSERT_LT(processed.size(), original.size());

    std::string out;
    err = Filework::Common::readFileData(path("target.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, processed);
}

// ---------------- Small / medium / large files ----------------
// Sizes: 10 bytes (small), 3000 bytes (medium), 90000 bytes (large)

TEST_P(Components_FileworkCommonSizes, SaveAndReadRoundTrip) {
    const std::size_t size = GetParam();
    const std::string content = makeContent(size);

    auto err = Filework::Common::saveFileData(path("sized.bin"), content);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(std::filesystem::file_size(path("sized.bin")), size);

    std::string out;
    err = Filework::Common::readFileData(path("sized.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out.size(), size);
    ASSERT_EQ(out, content);
}

TEST_P(Components_FileworkCommonSizes, AppendKeepsExistingAndAddsNewData) {
    const std::size_t size = GetParam();
    const std::string content = makeContent(size);

    auto err = Filework::Common::saveFileData(path("sized.bin"), content);
    ASSERT_TRUE(err) << err.getDetailText();
    err = Filework::Common::appendFileData(path("sized.bin"), content);
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(std::filesystem::file_size(path("sized.bin")), size * 2);

    std::string out;
    err = Filework::Common::readFileData(path("sized.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content + content);
}

TEST_P(Components_FileworkCommonSizes, StreamReadReturnsAllData) {
    const std::size_t size = GetParam();
    const std::string content = makeContent(size);
    createFile("sized.bin", content);

    std::string out;
    std::size_t callCount = 0;
    auto err = Filework::Common::streamReadFileData(
        path("sized.bin"),
        [&out, &callCount](const std::string_view& fileData, bool) -> bool {
            ++callCount;
            out.append(fileData.data(), fileData.size());
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out.size(), size);
    ASSERT_EQ(out, content);
    ASSERT_GE(callCount, 1u);
    if (size > static_cast<std::size_t>(Filework::Common::getBufferSize())) {
        ASSERT_GT(callCount, 1u);
    }
}

TEST_P(Components_FileworkCommonSizes, StreamWriteReplacesFileData) {
    const std::size_t size = GetParam();
    const std::string content = makeContent(size);
    createFile("sized.bin", "old data");

    std::size_t written = 0;
    auto err = Filework::Common::streamWriteFileData(
        path("sized.bin"),
        [&content, &written](char* writableBuffer, std::streamsize& writeCount) -> bool {
            const auto bytesLeft = std::min<std::size_t>(
                static_cast<std::size_t>(Filework::Common::getBufferSize()),
                content.size() - written);
            std::copy_n(content.data() + written, bytesLeft, writableBuffer);
            written += bytesLeft;
            writeCount = static_cast<std::streamsize>(bytesLeft);
            return bytesLeft > 0;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(std::filesystem::file_size(path("sized.bin")), size);

    std::string out;
    err = Filework::Common::readFileData(path("sized.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out, content);
}

TEST_P(Components_FileworkCommonSizes, StreamTransformReplacesAllSymbols) {
    const std::size_t size = GetParam();
    const std::string content = makeContent(size);
    const std::string expected = toUpper(content);
    createFile("sized.bin", content);

    std::size_t callCount = 0;
    auto err = Filework::Common::streamTransformFileData(
        path("sized.bin"),
        [&callCount](const std::string_view& fileData, std::string& replaceData, bool) -> bool {
            ++callCount;
            replaceData = toUpper(fileData);
            return true;
        });

    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_GE(callCount, 1u);
    if (size > static_cast<std::size_t>(Filework::Common::getBufferSize())) {
        ASSERT_GT(callCount, 1u);
    }
    ASSERT_EQ(std::filesystem::file_size(path("sized.bin")), size);

    std::string out;
    err = Filework::Common::readFileData(path("sized.bin"), out);
    ASSERT_TRUE(err) << err.getDetailText();
    ASSERT_EQ(out.size(), size);
    ASSERT_EQ(out, expected);
}

INSTANTIATE_TEST_SUITE_P(
    SmallMediumLarge,
    Components_FileworkCommonSizes,
    ::testing::Values(std::size_t{10}, std::size_t{3000}, std::size_t{90000}),
    [](const ::testing::TestParamInfo<Components_FileworkCommonSizes::ParamType>& info) {
        return std::to_string(info.param) + "Bytes";
    });
