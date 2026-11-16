#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include <stdint.h>

#include <Components/ExtraClasses/Error.h>

namespace Filework {

/**
 * @brief The FileTemplate class File
 *
 * File layout:
 * [header: name | key-value delim | value | entry delim | ...]
 * [header-data delimiter]
 * [data]
 */
class FileTemplate : public ExtraClasses::ErrorUserBase<ExtraClasses::ErrorObject>
{
public:
    using byteArray_t = std::vector<uint8_t>;
    using fileHeader_t = std::unordered_map<std::string, byteArray_t>;

    // Called before / after file opening / closing
    virtual ExtraClasses::ErrorObject filePrepare(const std::string_view& iFile, bool isReading);

    // Called after file reading completing or error during process
    virtual ExtraClasses::ErrorObject fileCallback(const std::string_view& iFile, bool isReading);

    // Generate map with file metadata contents
    virtual fileHeader_t                generateHeader() const;
    virtual ExtraClasses::ErrorObject   processHeader(const fileHeader_t& hdr);

    // Used to save / load file
    ExtraClasses::ErrorObject saveFile(const std::string_view& filePath, bool secureSave = false);
    ExtraClasses::ErrorObject readFile(const std::string_view& filePath);

private:
    std::size_t m_bufferSize {9128};
    byteArray_t m_headerKeyValueDelimiter {0x1F};
    byteArray_t m_headerEntryDelimiter {0x1E};
    byteArray_t m_headerDataDelimiter = byteArray_t(32, 0x0);
    std::size_t m_expectedHeaderSize {0};

    static bool containsSequence(const byteArray_t& data, const byteArray_t& sequence);
    static bool containsSequence(const std::string& data, const byteArray_t& sequence);

    ExtraClasses::ErrorObject validateDelimiters() const;
    ExtraClasses::ErrorObject serializeHeader(const fileHeader_t& hdr, byteArray_t& output) const;
    ExtraClasses::ErrorObject parseHeader(const std::string_view& input, fileHeader_t& hdr) const;

protected:
    void setReadBufferSize(const std::size_t bufferSize);
    ExtraClasses::ErrorObject setHeaderDelimeters(byteArray_t&& keyValueDelim, byteArray_t&& entryDelim);
    ExtraClasses::ErrorObject setHeaderDataDelimiter(byteArray_t&& delim);
    void setExpectedHeaderSize(const std::size_t headerSize);

    std::size_t getReadBufferSize() const;
    const byteArray_t& getHeaderKeyValueDelimiter() const;
    const byteArray_t& getHeaderEntryDelimiter() const;
    const byteArray_t& getHeaderDataDelimiter() const;
    std::size_t getExpectedHeaderSize() const;

    /**
     * @brief processReadDataPart   Process data part read from file
     * @param readBuffer            Buffer with data read
     * @return                      Must return true if read succeed
     */
    virtual bool processReadDataPart(const std::string_view& readBuffer) = 0;

    /**
     * @brief processSaveDataPart   Requrest bytes to save
     * @param writeBuffer
     * @param bufferSize
     * @return
     */
    virtual std::optional<int64_t> processSaveDataPart(char* writeBuffer,
                                     const std::size_t bufferSize) const = 0;
};

}