#include <gtest/gtest.h>

#include <Components/Filework/FileTemplate.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

class MinimalFileTemplate : public Filework::FileTemplate {
public:
    using Filework::FileTemplate::getExpectedHeaderSize;
    using Filework::FileTemplate::getHeaderDataDelimiter;
    using Filework::FileTemplate::getHeaderEntryDelimiter;
    using Filework::FileTemplate::getHeaderKeyValueDelimiter;
    using Filework::FileTemplate::getReadBufferSize;
    using Filework::FileTemplate::setExpectedHeaderSize;
    using Filework::FileTemplate::setHeaderDataDelimiter;
    using Filework::FileTemplate::setHeaderDelimeters;
    using Filework::FileTemplate::setReadBufferSize;

    std::string readData;
    std::string saveData;
    mutable std::size_t savePos {0};
    mutable int savePartCalls {0};
    int readPartCalls {0};

    bool callProcessReadDataPart(const std::string_view& buffer) {
        return processReadDataPart(buffer);
    }

    std::optional<int64_t> callProcessSaveDataPart(const std::size_t bufferSize,
                                                   std::string* writtenData = nullptr) const {
        std::vector<char> buffer(bufferSize, '\0');
        const auto result = processSaveDataPart(buffer.data(), bufferSize);
        if (writtenData && result.has_value() && result.value() > 0) {
            writtenData->append(buffer.data(), static_cast<std::size_t>(result.value()));
        }
        return result;
    }

protected:
    bool processReadDataPart(const std::string_view& readBuffer) override {
        ++readPartCalls;
        readData.append(readBuffer);
        return true;
    }

    std::optional<int64_t> processSaveDataPart(char* writeBuffer, const std::size_t bufferSize) const override {
        ++savePartCalls;
        if (savePos >= saveData.size()) {
            return std::nullopt;
        }
        const auto chunk = std::min({saveData.size() - savePos, bufferSize, std::size_t{16}});
        std::copy_n(saveData.data() + savePos, chunk, writeBuffer);
        savePos += chunk;
        return static_cast<int64_t>(chunk);
    }
};

class HeaderedFileTemplate : public MinimalFileTemplate {
public:
    Filework::FileTemplate::fileHeader_t headerToWrite;

    Filework::FileTemplate::fileHeader_t generateHeader() const override {
        return headerToWrite;
    }
};

class CapturingFileTemplate : public MinimalFileTemplate {
public:
    Filework::FileTemplate::fileHeader_t lastHeader;
    int processHeaderCalls {0};
    bool processHeaderFails {false};

    ExtraClasses::ErrorObject processHeader(const fileHeader_t& hdr) override {
        ++processHeaderCalls;
        lastHeader = hdr;
        if (processHeaderFails) {
            return {ExtraClasses::LogicalInvalidValue, "process header failed"};
        }
        return ExtraClasses::Ok;
    }
};

class RecordingFileTemplate : public MinimalFileTemplate {
public:
    bool prepareFails {false};
    bool callbackFails {false};
    int prepareCalls {0};
    int callbackCalls {0};
    int processHeaderCalls {0};

    ExtraClasses::ErrorObject filePrepare(const std::string_view&, bool) override {
        ++prepareCalls;
        if (prepareFails) {
            return {ExtraClasses::SystemFileError, "prepare failed"};
        }
        return ExtraClasses::Ok;
    }

    ExtraClasses::ErrorObject fileCallback(const std::string_view&, bool) override {
        ++callbackCalls;
        if (callbackFails) {
            return {ExtraClasses::SystemFileError, "callback failed"};
        }
        return ExtraClasses::Ok;
    }

    ExtraClasses::ErrorObject processHeader(const fileHeader_t&) override {
        ++processHeaderCalls;
        return ExtraClasses::Ok;
    }
};

} // namespace

class Components_FileworkFileTemplate : public ::testing::Test {
protected:
    using byteArray_t = Filework::FileTemplate::byteArray_t;
    using fileHeader_t = Filework::FileTemplate::fileHeader_t;

    const std::string testDir = "filetemplate_test_dir";
    const byteArray_t keyValueDelimiter {0x11, 0x12};
    const byteArray_t entryDelimiter {0x21, 0x22};
    const byteArray_t dataDelimiter = byteArray_t(32, 0x0);

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

    void createFile(const std::string& name, const std::string& content) const {
        std::ofstream file(path(name), std::ios_base::binary | std::ios_base::trunc);
        file.write(content.data(), static_cast<std::streamsize>(content.size()));
    }

    void appendBytes(std::string& target, const byteArray_t& bytes) const {
        target.append(reinterpret_cast<const char*>(bytes.data()),
                      static_cast<std::streamsize>(bytes.size()));
    }

    std::string serializeHeader(
        const std::vector<std::pair<std::string, std::string>>& entries) const {
        std::string result;
        for (const auto& [name, value] : entries) {
            result += name;
            appendBytes(result, keyValueDelimiter);
            result += value;
            appendBytes(result, entryDelimiter);
        }
        return result;
    }

    // File body: serialized header + header-data delimiter + payload
    void createTemplatedFile(const std::string& name,
                             const std::string& payload,
                             const std::string& headerData = {}) const {
        std::string content = headerData;
        appendBytes(content, dataDelimiter);
        content += payload;
        createFile(name, content);
    }

    std::string readFileBytes(const std::string& name) const {
        std::ifstream file(path(name), std::ios_base::binary | std::ios_base::in);
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    void configureDelimiters(MinimalFileTemplate& fileTemplate) const {
        auto headerErr = fileTemplate.setHeaderDelimeters(byteArray_t(keyValueDelimiter),
                                                          byteArray_t(entryDelimiter));
        ASSERT_TRUE(headerErr) << headerErr.getDetailText();
        auto dataErr = fileTemplate.setHeaderDataDelimiter(byteArray_t(dataDelimiter));
        ASSERT_TRUE(dataErr) << dataErr.getDetailText();
    }

    void prepareForRead(MinimalFileTemplate& fileTemplate, std::size_t readBufferSize = 4) const {
        configureDelimiters(fileTemplate);
        fileTemplate.setReadBufferSize(readBufferSize);
    }
};

// ---------------- filePrepare ----------------

TEST_F(Components_FileworkFileTemplate, FilePrepare_ReturnsOk) {
    MinimalFileTemplate fileTemplate;

    auto readErr = fileTemplate.filePrepare(path("any.bin"), true);
    ASSERT_TRUE(readErr) << readErr.getDetailText();

    auto writeErr = fileTemplate.filePrepare(path("any.bin"), false);
    ASSERT_TRUE(writeErr) << writeErr.getDetailText();
}

// ---------------- fileCallback ----------------

TEST_F(Components_FileworkFileTemplate, FileCallback_ReturnsOk) {
    MinimalFileTemplate fileTemplate;

    auto readErr = fileTemplate.fileCallback(path("any.bin"), true);
    ASSERT_TRUE(readErr) << readErr.getDetailText();

    auto writeErr = fileTemplate.fileCallback(path("any.bin"), false);
    ASSERT_TRUE(writeErr) << writeErr.getDetailText();
}

// ---------------- generateHeader / processHeader ----------------

TEST_F(Components_FileworkFileTemplate, GenerateHeader_DefaultIsEmpty) {
    MinimalFileTemplate fileTemplate;

    const auto header = fileTemplate.generateHeader();
    ASSERT_TRUE(header.empty());
}

TEST_F(Components_FileworkFileTemplate, ProcessHeader_ReturnsOk) {
    MinimalFileTemplate fileTemplate;

    fileHeader_t header;
    header["name"] = {1, 2, 3};
    header["empty"] = {};

    auto err = fileTemplate.processHeader(header);
    ASSERT_TRUE(err) << err.getDetailText();

    auto emptyErr = fileTemplate.processHeader({});
    ASSERT_TRUE(emptyErr) << emptyErr.getDetailText();
}

// ---------------- setters / getters ----------------

TEST_F(Components_FileworkFileTemplate, SetReadBufferSize_UpdatesValue) {
    MinimalFileTemplate fileTemplate;

    EXPECT_NO_THROW(fileTemplate.setReadBufferSize(16));
    EXPECT_EQ(fileTemplate.getReadBufferSize(), 16u);

    EXPECT_NO_THROW(fileTemplate.setReadBufferSize(0));
    EXPECT_EQ(fileTemplate.getReadBufferSize(), 0u);

    EXPECT_NO_THROW(fileTemplate.setReadBufferSize(4096));
    EXPECT_EQ(fileTemplate.getReadBufferSize(), 4096u);
}

TEST_F(Components_FileworkFileTemplate, SetExpectedHeaderSize_UpdatesValue) {
    MinimalFileTemplate fileTemplate;

    EXPECT_NO_THROW(fileTemplate.setExpectedHeaderSize(0));
    EXPECT_EQ(fileTemplate.getExpectedHeaderSize(), 0u);

    EXPECT_NO_THROW(fileTemplate.setExpectedHeaderSize(4));
    EXPECT_EQ(fileTemplate.getExpectedHeaderSize(), 4u);
}

TEST_F(Components_FileworkFileTemplate, DefaultDelimiters_AreValid) {
    MinimalFileTemplate fileTemplate;

    EXPECT_FALSE(fileTemplate.getHeaderKeyValueDelimiter().empty());
    EXPECT_FALSE(fileTemplate.getHeaderEntryDelimiter().empty());
    EXPECT_NE(fileTemplate.getHeaderKeyValueDelimiter(),
              fileTemplate.getHeaderEntryDelimiter());
    EXPECT_EQ(fileTemplate.getHeaderDataDelimiter(), byteArray_t(32, 0x0));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDelimeters_AcceptsValidValue) {
    MinimalFileTemplate fileTemplate;

    auto err = fileTemplate.setHeaderDelimeters(byteArray_t{0xAA, 0xBB},
                                                byteArray_t{0xCC, 0xDD});
    ASSERT_TRUE(err) << err.getDetailText();
    EXPECT_EQ(fileTemplate.getHeaderKeyValueDelimiter(), (byteArray_t{0xAA, 0xBB}));
    EXPECT_EQ(fileTemplate.getHeaderEntryDelimiter(), (byteArray_t{0xCC, 0xDD}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDelimeters_RejectsEmptyDelimiter) {
    MinimalFileTemplate fileTemplate;

    EXPECT_FALSE(fileTemplate.setHeaderDelimeters({}, byteArray_t{0xCC}));
    EXPECT_FALSE(fileTemplate.setHeaderDelimeters(byteArray_t{0xAA}, {}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDelimeters_RejectsEqualDelimiters) {
    MinimalFileTemplate fileTemplate;

    EXPECT_FALSE(fileTemplate.setHeaderDelimeters(byteArray_t{0xAA, 0xBB},
                                                  byteArray_t{0xAA, 0xBB}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDelimeters_RejectsOverlappedDelimiters) {
    MinimalFileTemplate fileTemplate;

    EXPECT_FALSE(fileTemplate.setHeaderDelimeters(byteArray_t{0xAA},
                                                  byteArray_t{0xAA, 0xBB}));
    EXPECT_FALSE(fileTemplate.setHeaderDelimeters(byteArray_t{0xAA, 0xBB},
                                                  byteArray_t{0xBB}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDelimeters_KeepsPreviousValueOnError) {
    MinimalFileTemplate fileTemplate;

    const auto previousKeyValue = fileTemplate.getHeaderKeyValueDelimiter();
    const auto previousEntry = fileTemplate.getHeaderEntryDelimiter();

    EXPECT_FALSE(fileTemplate.setHeaderDelimeters(byteArray_t{0xAA}, {}));
    EXPECT_EQ(fileTemplate.getHeaderKeyValueDelimiter(), previousKeyValue);
    EXPECT_EQ(fileTemplate.getHeaderEntryDelimiter(), previousEntry);
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDataDelimiter_AcceptsValidValue) {
    MinimalFileTemplate fileTemplate;

    auto err = fileTemplate.setHeaderDataDelimiter(byteArray_t{0x33, 0x44, 0x55});
    ASSERT_TRUE(err) << err.getDetailText();
    EXPECT_EQ(fileTemplate.getHeaderDataDelimiter(), (byteArray_t{0x33, 0x44, 0x55}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDataDelimiter_RejectsInvalidValue) {
    MinimalFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);

    EXPECT_FALSE(fileTemplate.setHeaderDataDelimiter({}));
    EXPECT_FALSE(fileTemplate.setHeaderDataDelimiter(byteArray_t{0x11}));
    EXPECT_FALSE(fileTemplate.setHeaderDataDelimiter(byteArray_t{0x21, 0x22}));
    EXPECT_FALSE(fileTemplate.setHeaderDataDelimiter(byteArray_t{0x11, 0x12}));
}

TEST_F(Components_FileworkFileTemplate, SetHeaderDataDelimiter_KeepsPreviousValueOnError) {
    MinimalFileTemplate fileTemplate;

    const auto previousDelimiter = fileTemplate.getHeaderDataDelimiter();

    EXPECT_FALSE(fileTemplate.setHeaderDataDelimiter({}));
    EXPECT_EQ(fileTemplate.getHeaderDataDelimiter(), previousDelimiter);
}

// ---------------- processReadDataPart / processSaveDataPart ----------------

TEST_F(Components_FileworkFileTemplate, ProcessReadDataPart_AccumulatesData) {
    MinimalFileTemplate fileTemplate;

    ASSERT_TRUE(fileTemplate.callProcessReadDataPart(std::string_view("abc")));
    ASSERT_TRUE(fileTemplate.callProcessReadDataPart(std::string_view("de")));

    ASSERT_EQ(fileTemplate.readData, "abcde");
    ASSERT_EQ(fileTemplate.readPartCalls, 2);
}

TEST_F(Components_FileworkFileTemplate, ProcessSaveDataPart_ReturnsChunksUntilExhausted) {
    MinimalFileTemplate fileTemplate;
    fileTemplate.saveData = "abcdef";

    std::string written;

    auto first = fileTemplate.callProcessSaveDataPart(2, &written);
    ASSERT_TRUE(first.has_value());
    ASSERT_EQ(first.value(), 2);

    auto second = fileTemplate.callProcessSaveDataPart(2, &written);
    ASSERT_TRUE(second.has_value());
    ASSERT_EQ(second.value(), 2);

    auto third = fileTemplate.callProcessSaveDataPart(2, &written);
    ASSERT_TRUE(third.has_value());
    ASSERT_EQ(third.value(), 2);

    auto end = fileTemplate.callProcessSaveDataPart(2, &written);
    ASSERT_FALSE(end.has_value());
    ASSERT_EQ(fileTemplate.savePartCalls, 4);
    ASSERT_EQ(written, fileTemplate.saveData);
}

// ---------------- saveFile ----------------

TEST_F(Components_FileworkFileTemplate, SaveFile_WritesAllPartsAndCallsCallback) {
    createFile("target.bin", "old content");

    MinimalFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "hello";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.savePartCalls, 2); // one data part + end marker
    ASSERT_TRUE(fileTemplate.getError().isOk()) << fileTemplate.getError().getDetailText();
}

TEST_F(Components_FileworkFileTemplate, SaveFile_WritesDataDelimiterBeforePayload) {
    createFile("target.bin", "old content");

    MinimalFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "hello";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    std::string expected;
    appendBytes(expected, dataDelimiter);
    expected += "hello";

    ASSERT_EQ(readFileBytes("target.bin"), expected);
}

TEST_F(Components_FileworkFileTemplate, SaveFile_WritesHeaderWithBothDelimiters) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.headerToWrite = {{"HDR", {0x01, 0x02}}};
    fileTemplate.saveData = "payload";
    fileTemplate.setExpectedHeaderSize(9); // "HDR" + kv(2) + 2 bytes + entry(2)

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    std::string expected;
    expected += "HDR";
    appendBytes(expected, keyValueDelimiter);
    expected.push_back(static_cast<char>(0x01));
    expected.push_back(static_cast<char>(0x02));
    appendBytes(expected, entryDelimiter);
    appendBytes(expected, dataDelimiter);
    expected += "payload";

    ASSERT_EQ(readFileBytes("target.bin"), expected);
}

TEST_F(Components_FileworkFileTemplate, SaveFile_SortsHeaderEntriesByName) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.headerToWrite = {{"B", {0x02}}, {"A", {0x01}}};
    fileTemplate.saveData = "payload";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    std::string expected;
    expected += "A";
    appendBytes(expected, keyValueDelimiter);
    expected.push_back(0x01);
    appendBytes(expected, entryDelimiter);
    expected += "B";
    appendBytes(expected, keyValueDelimiter);
    expected.push_back(0x02);
    appendBytes(expected, entryDelimiter);
    appendBytes(expected, dataDelimiter);
    expected += "payload";

    ASSERT_EQ(readFileBytes("target.bin"), expected);
}

TEST_F(Components_FileworkFileTemplate, SaveFile_HeaderSizeExceedReportsFailure) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.headerToWrite = {{"HDR", {0x01, 0x02}}};
    fileTemplate.saveData = "payload";
    fileTemplate.setExpectedHeaderSize(1);

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Expected header size exceed");
}

TEST_F(Components_FileworkFileTemplate, SaveFile_RejectsHeaderNameWithDelimiter) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    std::string nameWithDelimiter {"HD"};
    appendBytes(nameWithDelimiter, keyValueDelimiter);
    nameWithDelimiter += "R";
    fileTemplate.headerToWrite = {{nameWithDelimiter, {0x01}}};
    fileTemplate.saveData = "payload";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
}

TEST_F(Components_FileworkFileTemplate, SaveFile_RejectsHeaderValueWithDelimiter) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.headerToWrite = {{"HDR", {0x01, 0x21, 0x22}}};
    fileTemplate.saveData = "payload";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
}

TEST_F(Components_FileworkFileTemplate, SaveFile_RejectsHeaderValueWithDataDelimiter) {
    createFile("target.bin", "");

    HeaderedFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.headerToWrite = {{"HDR", byteArray_t(32, 0x0)}};
    fileTemplate.saveData = "payload";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
}

TEST_F(Components_FileworkFileTemplate, SaveFile_EmptyHeaderStillWritesDataDelimiter) {
    createFile("target.bin", "");

    MinimalFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "payload";

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    std::string expected;
    appendBytes(expected, dataDelimiter);
    expected += "payload";

    ASSERT_EQ(readFileBytes("target.bin"), expected);
}

TEST_F(Components_FileworkFileTemplate, SaveFile_MissingFileReportsFailure) {
    MinimalFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "hello";

    auto err = fileTemplate.saveFile(path("missing.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkFileTemplate, SaveFile_ReturnsPrepareError) {
    createFile("target.bin", "old content");

    RecordingFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "hello";
    fileTemplate.prepareFails = true;

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(fileTemplate.prepareCalls, 1);
    ASSERT_EQ(fileTemplate.callbackCalls, 0);
    ASSERT_EQ(fileTemplate.savePartCalls, 0);
}

TEST_F(Components_FileworkFileTemplate, SaveFile_ReturnsCallbackError) {
    createFile("target.bin", "old content");

    RecordingFileTemplate fileTemplate;
    configureDelimiters(fileTemplate);
    fileTemplate.saveData = "hello";
    fileTemplate.callbackFails = true;

    auto err = fileTemplate.saveFile(path("target.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(fileTemplate.prepareCalls, 1);
    ASSERT_EQ(fileTemplate.callbackCalls, 1);
}

// ---------------- readFile ----------------

TEST_F(Components_FileworkFileTemplate, ReadFile_ReadsPayload) {
    const std::string payload = "abcdefgh";
    createTemplatedFile("source.bin", payload);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate, 4);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.readData, payload);
    ASSERT_GE(fileTemplate.readPartCalls, 1);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_ReadsExactBytesFromDisk) {
    std::string payload;
    payload.push_back('\0');
    payload.push_back(static_cast<char>(0x01));
    payload.push_back(static_cast<char>(0xFE));
    payload.push_back(static_cast<char>(0xFF));
    payload += "end";
    createTemplatedFile("source.bin", payload);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate, 3);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.readData.size(), payload.size());
    ASSERT_EQ(fileTemplate.readData, payload);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_CallsHooksAndProcessHeader) {
    const std::string payload = "payload";
    createTemplatedFile("source.bin", payload);

    RecordingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.readData, payload);
    ASSERT_EQ(fileTemplate.prepareCalls, 1);
    ASSERT_EQ(fileTemplate.callbackCalls, 1);
    ASSERT_EQ(fileTemplate.processHeaderCalls, 1);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_SeparatesHeaderFromPayload) {
    const std::string headerData = serializeHeader({{"A", "first"}, {"B", "second"}});
    const std::string payload = "payload";
    createTemplatedFile("source.bin", payload, headerData);

    CapturingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.processHeaderCalls, 1);
    ASSERT_EQ(fileTemplate.lastHeader.size(), 2u);
    ASSERT_EQ(fileTemplate.lastHeader.at("A"), byteArray_t({'f', 'i', 'r', 's', 't'}));
    ASSERT_EQ(fileTemplate.lastHeader.at("B"), byteArray_t({'s', 'e', 'c', 'o', 'n', 'd'}));
    ASSERT_EQ(fileTemplate.readData, payload);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_DoesNotTreatPayloadAsHeader) {
    const std::string headerData = serializeHeader({{"HDR", "value"}});
    const std::string payload = headerData + "payload" + headerData;
    createTemplatedFile("source.bin", payload, headerData);

    CapturingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.lastHeader.size(), 1u);
    ASSERT_TRUE(fileTemplate.lastHeader.count("HDR"));
    ASSERT_EQ(fileTemplate.readData, payload);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_PayloadCanContainDataDelimiter) {
    const std::string headerData = serializeHeader({{"HDR", "value"}});
    std::string payload = "prefix";
    appendBytes(payload, dataDelimiter);
    payload += "suffix";
    createTemplatedFile("source.bin", payload, headerData);

    CapturingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.lastHeader.size(), 1u);
    ASSERT_EQ(fileTemplate.readData, payload);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_EmptyHeaderIsAllowed) {
    const std::string payload = "payload";
    createTemplatedFile("source.bin", payload);

    CapturingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_TRUE(err) << err.getDetailText();

    ASSERT_EQ(fileTemplate.processHeaderCalls, 1);
    ASSERT_TRUE(fileTemplate.lastHeader.empty());
    ASSERT_EQ(fileTemplate.readData, payload);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_MissingDataDelimiterReportsFailure) {
    createFile("source.bin", "header without data delimiter");

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Header-data delimiter not found");
}

TEST_F(Components_FileworkFileTemplate, ReadFile_MissingEntryDelimiterReportsFailure) {
    std::string headerData = "HDR";
    appendBytes(headerData, keyValueDelimiter);
    headerData += "value";
    createTemplatedFile("source.bin", "payload", headerData);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Header entry delimiter not found");
}

TEST_F(Components_FileworkFileTemplate, ReadFile_MissingKeyValueDelimiterReportsFailure) {
    std::string headerData = "HDR";
    appendBytes(headerData, entryDelimiter);
    createTemplatedFile("source.bin", "payload", headerData);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Header key-value delimiter not found");
}

TEST_F(Components_FileworkFileTemplate, ReadFile_DuplicatedHeaderEntryReportsFailure) {
    const std::string headerData = serializeHeader({{"HDR", "first"}, {"HDR", "second"}});
    createTemplatedFile("source.bin", "payload", headerData);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Duplicated header entry name");
}

TEST_F(Components_FileworkFileTemplate, ReadFile_HeaderSizeExceedReportsFailure) {
    const std::string headerData = serializeHeader({{"HDR", "value"}});
    createTemplatedFile("source.bin", "payload", headerData);

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);
    fileTemplate.setExpectedHeaderSize(1);

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "FileTemplate: Expected header size exceed");
}

TEST_F(Components_FileworkFileTemplate, ReadFile_ReturnsProcessHeaderError) {
    const std::string headerData = serializeHeader({{"HDR", "value"}});
    createTemplatedFile("source.bin", "payload", headerData);

    CapturingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);
    fileTemplate.processHeaderFails = true;

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(err.getDetailText(), "process header failed");
    ASSERT_EQ(fileTemplate.processHeaderCalls, 1);
}

TEST_F(Components_FileworkFileTemplate, ReadFile_MissingFileReportsFailure) {
    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("missing.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_FALSE(err.getDetailText().empty());
}

TEST_F(Components_FileworkFileTemplate, ReadFile_DirectoryReportsFailure) {
    std::filesystem::create_directories(path("subdir"));

    MinimalFileTemplate fileTemplate;
    prepareForRead(fileTemplate);

    auto err = fileTemplate.readFile(path("subdir"));
    ASSERT_FALSE(err) << err.getDetailText();
}

TEST_F(Components_FileworkFileTemplate, ReadFile_ReturnsCallbackError) {
    createTemplatedFile("source.bin", "payload");

    RecordingFileTemplate fileTemplate;
    prepareForRead(fileTemplate);
    fileTemplate.callbackFails = true;

    auto err = fileTemplate.readFile(path("source.bin"));
    ASSERT_FALSE(err) << err.getDetailText();
    ASSERT_EQ(fileTemplate.prepareCalls, 1);
    ASSERT_EQ(fileTemplate.callbackCalls, 1);
}

TEST_F(Components_FileworkFileTemplate, SaveAndRead_RoundTripsHeaderAndPayload) {
    createFile("target.bin", "");

    HeaderedFileTemplate writer;
    configureDelimiters(writer);
    writer.headerToWrite = {{"A", {0x01, 0x02}}, {"B", {0x03}}};
    writer.saveData = "payload";

    auto saveErr = writer.saveFile(path("target.bin"));
    ASSERT_TRUE(saveErr) << saveErr.getDetailText();

    CapturingFileTemplate reader;
    prepareForRead(reader);

    auto readErr = reader.readFile(path("target.bin"));
    ASSERT_TRUE(readErr) << readErr.getDetailText();

    ASSERT_EQ(reader.lastHeader, writer.headerToWrite);
    ASSERT_EQ(reader.readData, writer.saveData);
}
