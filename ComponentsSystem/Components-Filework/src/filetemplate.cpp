#include "filetemplate.hpp"

#include "temporaryfile.hpp"
#include "common.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <filesystem>

#include <cstring>

namespace Filework {

ExtraClasses::ErrorObject FileTemplate::filePrepare([[maybe_unused]] const std::string_view &iFile,
                               [[maybe_unused]] bool isReading)
{
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject FileTemplate::fileCallback([[maybe_unused]] const std::string_view &iFile,
                                [[maybe_unused]] bool isReading)
{
    return ExtraClasses::Ok;
}

FileTemplate::fileHeader_t FileTemplate::generateHeader() const
{
    return {};
}

ExtraClasses::ErrorObject FileTemplate::processHeader([[maybe_unused]] const fileHeader_t &hdr)
{
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject FileTemplate::saveFile(const std::string_view &filePath, bool secureSave)
{
    auto prepareResult = filePrepare(filePath, false);
    if (!prepareResult.isOk()) { return prepareResult; }

    auto delimitersResult = validateDelimiters();
    if (!delimitersResult.isOk()) { return delimitersResult; }

    // Check file for validance
    auto isacc = Common::isAccessable(filePath);
    if (!isacc.isOk()) {
        return isacc;
    }
    if (std::filesystem::status(filePath).type() == std::filesystem::file_type::directory) {
        return {ExtraClasses::SystemFileError,
                std::string("Is directory. Path: \"") + std::string(filePath) + "\""};
    }
    if (!Common::isReadable(filePath)) {
        return {ExtraClasses::SystemFileError,
                std::string("File not readable. Path: \"") + std::string(filePath) + "\""};
    }

    TemporaryFile outputFile;
    outputFile.setSecureMode(secureSave);
    if (!outputFile.setTargetFile(std::string(filePath), false)) {
        m_error = {ExtraClasses::ErrorCode::SystemFileError,
                   std::string("FileTemplate: Failed to open target file for save: ") + strerror(errno)};
        return m_error;
    }

    byteArray_t headerData;
    auto serializeResult = serializeHeader(generateHeader(), headerData);
    if (!serializeResult.isOk()) {
        m_error = serializeResult;
        return m_error;
    }
    if (m_expectedHeaderSize > 0 && headerData.size() > m_expectedHeaderSize) {
        m_error = {ExtraClasses::ErrorCode::LogicalInvalidValue,
                   "FileTemplate: Expected header size exceed"};
        return m_error;
    }

    if (!headerData.empty()) {
        outputFile.write(reinterpret_cast<const char*>(headerData.data()),
                         static_cast<std::streamsize>(headerData.size()));
    }
    outputFile.write(reinterpret_cast<const char*>(m_headerDataDelimiter.data()),
                     static_cast<std::streamsize>(m_headerDataDelimiter.size()));
    outputFile.flush();

    char buffer [m_bufferSize];
    while (!outputFile.bad()) {
        auto bytesToWriteOpt = processSaveDataPart(buffer, m_bufferSize);
        if (!bytesToWriteOpt.has_value()) { break; }
        if (bytesToWriteOpt.value() >= m_bufferSize ||
            bytesToWriteOpt.value() <= 0) {
            m_error = {ExtraClasses::ErrorCode::LogicalInvalidValue,
                       "FileTemplate: Internal error (invalid byte count to write)"};
            return m_error;
        }
        outputFile.write(buffer, bytesToWriteOpt.value());
        outputFile.flush();
    }
    outputFile.accept();
    return fileCallback(filePath, false);
}

ExtraClasses::ErrorObject FileTemplate::readFile(const std::string_view &filePath)
{
    auto prepareResult = filePrepare(filePath, true);
    if (!prepareResult.isOk()) { return prepareResult; }

    // Check file for validance
    auto isacc = Common::isAccessable(filePath);
    if (!isacc.isOk()) {
        return isacc;
    }
    if (std::filesystem::status(filePath).type() == std::filesystem::file_type::directory) {
        return {ExtraClasses::SystemFileError,
                std::string("Is directory. Path: \"") + std::string(filePath) + "\""};
    }
    if (!Common::isReadable(filePath)) {
        return {ExtraClasses::SystemFileError,
                std::string("File not readable. Path: \"") + std::string(filePath) + "\""};
    }

    std::ifstream iFile(std::string(filePath), std::ios::binary | std::ios::in);
    if (!iFile.is_open()) {
        return {ExtraClasses::SystemFileError,
                std::string("File open error. Path: \"") + std::string(filePath) + "\""};
    }

    // Read header separated from data by the header-data delimiter
    {
        auto delimitersResult = validateDelimiters();
        if (!delimitersResult.isOk()) { return delimitersResult; }

        std::string headerData;
        std::vector<char> window;
        window.reserve(m_headerDataDelimiter.size());

        const auto isHeaderDataDelimiter = [&]() {
            return window.size() == m_headerDataDelimiter.size() &&
                   std::equal(window.begin(), window.end(),
                              m_headerDataDelimiter.begin(),
                              [](const char lVal, const uint8_t rVal) {
                                  return static_cast<uint8_t>(lVal) == rVal;
                              });
        };

        bool isFound {false};
        char currentByte {};
        while (iFile.get(currentByte)) {
            window.push_back(currentByte);
            if (window.size() > m_headerDataDelimiter.size()) {
                headerData.push_back(window.front());
                window.erase(window.begin());
            }
            if (m_expectedHeaderSize > 0 && headerData.size() > m_expectedHeaderSize) {
                m_error = {ExtraClasses::ErrorCode::LogicalInvalidValue,
                           "FileTemplate: Expected header size exceed"};
                return m_error;
            }
            if (isHeaderDataDelimiter()) {
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            m_error = {ExtraClasses::ErrorCode::SystemFileError,
                       "FileTemplate: Header-data delimiter not found"};
            return m_error;
        }

        fileHeader_t header;
        auto parseResult = parseHeader(headerData, header);
        if (!parseResult.isOk()) {
            m_error = parseResult;
            return m_error;
        }
        auto processResult = processHeader(header);
        if (!processResult.isOk()) {
            m_error = processResult;
            return m_error;
        }
    }

    // If error with size occurs here, set COMPONENTS_READ_BUFFER_SIZE in CMake to 65535 or other size
    std::array<char, COMPONENTS_READ_BUFFER_SIZE> buffer;
    while (iFile) {
        iFile.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const std::streamsize bytesRead = iFile.gcount();

        if ((bytesRead <= 0) ||
            !processReadDataPart(std::string_view(buffer.data(),
                                                  static_cast<std::size_t>(bytesRead)))) {
            break;
        }
    }
    if (iFile.bad()) {
        return {ExtraClasses::SystemFileError,
                std::string("Critical I/O error. Path: \"") + std::string(filePath) + "\""};
    }
    if (iFile.fail() && !iFile.eof()) {
        return {ExtraClasses::SystemFileError,
                std::string("Logical read error. Path: \"") + std::string(filePath) + "\""};
    }

    iFile.close();
    return fileCallback(filePath, true);
}

bool FileTemplate::containsSequence(const byteArray_t &data, const byteArray_t &sequence)
{
    if (sequence.empty()) { return true; }
    return std::search(data.begin(), data.end(),
                       sequence.begin(), sequence.end()) != data.end();
}

bool FileTemplate::containsSequence(const std::string &data, const byteArray_t &sequence)
{
    if (sequence.empty()) { return true; }
    return std::search(data.begin(), data.end(),
                       sequence.begin(), sequence.end(),
                       [](const char lVal, const uint8_t rVal) {
                           return static_cast<uint8_t>(lVal) == rVal;
                       }) != data.end();
}

ExtraClasses::ErrorObject FileTemplate::validateDelimiters() const
{
    if (m_headerKeyValueDelimiter.empty()) {
        return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                "FileTemplate: Header key-value delimiter is empty"};
    }
    if (m_headerEntryDelimiter.empty()) {
        return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                "FileTemplate: Header entry delimiter is empty"};
    }
    if (m_headerDataDelimiter.empty()) {
        return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                "FileTemplate: Header-data delimiter is empty"};
    }

    const auto isOverlapped = [](const byteArray_t& first, const byteArray_t& second) {
        return containsSequence(first, second) || containsSequence(second, first);
    };

    if (isOverlapped(m_headerKeyValueDelimiter, m_headerEntryDelimiter)) {
        return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                "FileTemplate: Header delimiters must not overlap"};
    }
    if (isOverlapped(m_headerKeyValueDelimiter, m_headerDataDelimiter) ||
        isOverlapped(m_headerEntryDelimiter, m_headerDataDelimiter)) {
        return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                "FileTemplate: Header-data delimiter must not overlap header delimiters"};
    }
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject FileTemplate::serializeHeader(const fileHeader_t &hdr, byteArray_t &output) const
{
    auto validateResult = validateDelimiters();
    if (!validateResult.isOk()) { return validateResult; }

    std::vector<std::pair<std::string, const byteArray_t*>> entries;
    entries.reserve(hdr.size());
    for (const auto& [name, value] : hdr) {
        if (name.empty()) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header entry name is empty"};
        }
        if (containsSequence(name, m_headerKeyValueDelimiter) ||
            containsSequence(name, m_headerEntryDelimiter) ||
            containsSequence(name, m_headerDataDelimiter)) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header name contains a delimiter. Name: \"" + name + "\""};
        }
        if (containsSequence(value, m_headerKeyValueDelimiter) ||
            containsSequence(value, m_headerEntryDelimiter) ||
            containsSequence(value, m_headerDataDelimiter)) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header value contains a delimiter. Name: \"" + name + "\""};
        }
        entries.emplace_back(name, &value);
    }

    std::sort(entries.begin(), entries.end(),
              [](const auto& first, const auto& second) {
                  return first.first < second.first;
              });

    output.clear();
    for (const auto& [name, value] : entries) {
        output.insert(output.end(), name.begin(), name.end());
        output.insert(output.end(),
                      m_headerKeyValueDelimiter.begin(),
                      m_headerKeyValueDelimiter.end());
        output.insert(output.end(), value->begin(), value->end());
        output.insert(output.end(),
                      m_headerEntryDelimiter.begin(),
                      m_headerEntryDelimiter.end());
    }
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject FileTemplate::parseHeader(const std::string_view &input, fileHeader_t &hdr) const
{
    auto validateResult = validateDelimiters();
    if (!validateResult.isOk()) { return validateResult; }

    hdr.clear();
    std::size_t currentPos {};
    while (currentPos < input.size()) {
        const auto entryEndIt = std::search(
            input.begin() + static_cast<std::ptrdiff_t>(currentPos), input.end(),
            m_headerEntryDelimiter.begin(), m_headerEntryDelimiter.end(),
            [](const char lVal, const uint8_t rVal) {
                return static_cast<uint8_t>(lVal) == rVal;
            });
        if (entryEndIt == input.end()) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header entry delimiter not found"};
        }

        const auto entryEnd = static_cast<std::size_t>(entryEndIt - input.begin());
        const auto entry = input.substr(currentPos, entryEnd - currentPos);

        const auto keyValueIt = std::search(
            entry.begin(), entry.end(),
            m_headerKeyValueDelimiter.begin(), m_headerKeyValueDelimiter.end(),
            [](const char lVal, const uint8_t rVal) {
                return static_cast<uint8_t>(lVal) == rVal;
            });
        if (keyValueIt == entry.end()) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header key-value delimiter not found"};
        }

        const auto keyValuePos = static_cast<std::size_t>(keyValueIt - entry.begin());
        std::string name(entry.substr(0, keyValuePos));
        if (name.empty()) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Header entry name is empty"};
        }

        const auto valuePos = keyValuePos + m_headerKeyValueDelimiter.size();
        byteArray_t value(entry.begin() + static_cast<std::ptrdiff_t>(valuePos), entry.end());
        if (!hdr.emplace(std::move(name), std::move(value)).second) {
            return {ExtraClasses::ErrorCode::LogicalInvalidValue,
                    "FileTemplate: Duplicated header entry name"};
        }

        currentPos = entryEnd + m_headerEntryDelimiter.size();
    }
    return ExtraClasses::Ok;
}

void FileTemplate::setReadBufferSize(const std::size_t bufferSize)
{
    m_bufferSize = bufferSize;
}

ExtraClasses::ErrorObject FileTemplate::setHeaderDelimeters(byteArray_t &&keyValueDelim,
                                                            byteArray_t &&entryDelim)
{
    auto previousKeyValue = m_headerKeyValueDelimiter;
    auto previousEntry = m_headerEntryDelimiter;

    m_headerKeyValueDelimiter = std::move(keyValueDelim);
    m_headerEntryDelimiter = std::move(entryDelim);

    auto validationResult = validateDelimiters();
    if (!validationResult.isOk()) {
        m_headerKeyValueDelimiter = std::move(previousKeyValue);
        m_headerEntryDelimiter = std::move(previousEntry);
        m_error = validationResult;
        return validationResult;
    }
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject FileTemplate::setHeaderDataDelimiter(byteArray_t &&delim)
{
    auto previousDelimiter = m_headerDataDelimiter;

    m_headerDataDelimiter = std::move(delim);

    auto validationResult = validateDelimiters();
    if (!validationResult.isOk()) {
        m_headerDataDelimiter = std::move(previousDelimiter);
        m_error = validationResult;
        return validationResult;
    }
    return ExtraClasses::Ok;
}

void FileTemplate::setExpectedHeaderSize(const std::size_t headerSize)
{
    m_expectedHeaderSize = headerSize;
}

std::size_t FileTemplate::getReadBufferSize() const
{
    return m_bufferSize;
}

const FileTemplate::byteArray_t &FileTemplate::getHeaderKeyValueDelimiter() const
{
    return m_headerKeyValueDelimiter;
}

const FileTemplate::byteArray_t &FileTemplate::getHeaderEntryDelimiter() const
{
    return m_headerEntryDelimiter;
}

const FileTemplate::byteArray_t &FileTemplate::getHeaderDataDelimiter() const
{
    return m_headerDataDelimiter;
}

std::size_t FileTemplate::getExpectedHeaderSize() const
{
    return m_expectedHeaderSize;
}

}