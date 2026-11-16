#include "common.hpp"

#include "temporaryfile.hpp"

#include <cstring>
#include <regex>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace Filework::Common
{

std::streamsize getBufferSize() {
    return COMPONENTS_READ_BUFFER_SIZE;
}

std::vector<std::string> getContentPaths(const std::string_view &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return {};

    std::vector<std::string> result;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
        result.push_back(entry.path());
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> getContentNames(const std::string_view &dirPath, const std::string nameFilterRegexp)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return {};

    std::regex filterRegexp(nameFilterRegexp, std::regex::flag_type::_S_ECMAScript);

    std::vector<std::string> result;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt) {
        std::string fileName = entry.path().filename();
        if (std::regex_search(fileName, filterRegexp)) {
            result.push_back(fileName);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}


// ============================================================ //

bool isReadable(const std::string_view &objectPath)
{
    const auto p = stdfs::status(objectPath).permissions();
    return (stdfs::perms::none !=
            (p & (stdfs::perms::owner_read | stdfs::perms::group_read | stdfs::perms::others_read)));
}

bool isWritable(const std::string_view &objectPath)
{
    // Can be created
    const auto parentPerms = stdfs::status(objectPath).permissions();
    auto isParentWritable = (stdfs::perms::none !=
            (parentPerms & (stdfs::perms::owner_write | stdfs::perms::group_write | stdfs::perms::others_write)));
    if (!stdfs::exists(objectPath)) {
        return isParentWritable;
    }

    // Can be written
    const auto p = stdfs::status(objectPath).permissions();
    return (stdfs::perms::none !=
            (p & (stdfs::perms::owner_write | stdfs::perms::group_write | stdfs::perms::others_write)));
}

ExtraClasses::ErrorObject isAccessable(const std::string_view &objectPath)
{
    std::error_code ec;
    const auto st = stdfs::status(objectPath, ec);

    if (ec == std::errc::permission_denied) {
        return {ExtraClasses::SystemFileError,
                std::string("Permission denied (parent directory or file). Path: \"") + std::string(objectPath) + "\""};
    }

    if (ec) {
        return {ExtraClasses::SystemFileError,
                std::string("Filesystem error. Path: \"") +
                    std::string(objectPath) + "\", error: " + ec.message()};
    }

    if (st.type() == stdfs::file_type::not_found) {
        return {ExtraClasses::SystemFileError,
                std::string("Filesystem error. Path: \"") +
                    std::string(objectPath) + "\", error: " + ec.message()};
    }
    if (st.type() == stdfs::file_type::unknown) {
        return {ExtraClasses::SystemFileError,
                std::string("Invalid permissions. Path: \"") + std::string(objectPath) + "\""};
    }
    return ExtraClasses::Ok;
}

bool objectExist(const std::string_view &objectPath)
{
    return stdfs::exists(objectPath);
}

bool contains(const std::string_view &dirPath, const std::string_view &fileName)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return false;

    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
    {
        if (entry.path().filename() == fileName)
            return true;
    }
    return false;
}


// ============================================================ //


int16_t objectCount(const std::string_view &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t objCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt) {
        objCount++;
    }
    return objCount;
}

int16_t fileCount(const std::string_view &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t fileCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt) {
        if (stdfs::is_regular_file(entry))
            fileCount++;
    }
    return fileCount;
}

int16_t dirCount(const std::string_view &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t dirCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt) {
        if (stdfs::is_directory(entry))
            dirCount++;
    }
    return dirCount;
}


// ============================================================ //


ExtraClasses::ErrorObject readFileData(const std::string_view &filePath, std::string &output)
{
    auto isacc = isAccessable(filePath);
    if (!isacc.isOk()) { return isacc; }
    if (!isReadable(filePath)) { return {ExtraClasses::SystemFileError, std::string("File not readable: ") + std::string(filePath)}; }
    output.clear();
    std::error_code sizeError;
    const auto expectedSize = stdfs::file_size(filePath, sizeError);
    if (!sizeError) {
        output.reserve(expectedSize);
    }
    return streamReadFileData(filePath, [&output](const std::string_view& readBuffer, bool) -> bool {
        output.append(readBuffer.data(), readBuffer.size());
        return true;
    });
}

ExtraClasses::ErrorObject saveFileData(const std::string_view &filePath, const std::string_view &input)
{
    std::size_t currentInputPos {};
    return streamWriteFileData(filePath, [&input, &currentInputPos](char* writableBuffer, std::streamsize& writeCount) -> bool {
        auto bytesLeft = std::min(static_cast<std::size_t>(COMPONENTS_READ_BUFFER_SIZE), input.size() - currentInputPos);
        std::copy_n(input.data() + currentInputPos, bytesLeft, writableBuffer);
        currentInputPos += bytesLeft;
        writeCount = static_cast<std::streamsize>(bytesLeft);
        return bytesLeft > 0;
    });
}

ExtraClasses::ErrorObject appendFileData(const std::string_view &filePath, const std::string_view &input)
{
    try {
        std::ofstream oFile;
        oFile.exceptions(std::ios::failbit);
        oFile.open(std::string(filePath), std::ios_base::app | std::ios_base::out | std::ios_base::binary);
        if (!oFile.is_open()) {
            return {ExtraClasses::SystemFileError, "Failed to open file for appending"};
        }
        oFile.write(input.data(), input.size());
        oFile.flush();
        oFile.close();
    } catch (const std::exception& ex) {
        return {ExtraClasses::SystemFileError,
                std::string("Failed to append file data: ") + ex.what()};
    }
    return ExtraClasses::Ok;
}


// ============================================================ //


ExtraClasses::ErrorObject streamReadFileData(const std::string_view &filePath, fileReadCbk_t &&readCbk)
{
    // Check file for validance
    auto isacc = isAccessable(filePath);
    if (!isacc.isOk()) {
        return isacc;
    }
    if (stdfs::status(filePath).type() == stdfs::file_type::directory) {
        return {ExtraClasses::SystemFileError,
                std::string("Is directory. Path: \"") + std::string(filePath) + "\""};
    }
    if (!isReadable(filePath)) {
        return {ExtraClasses::SystemFileError,
                std::string("File not readable. Path: \"") + std::string(filePath) + "\""};
    }

    std::ifstream iFile;
    iFile.open(std::string(filePath), std::ios::binary | std::ios::in);
    if (!iFile.is_open()) {
        return {ExtraClasses::SystemFileError,
                std::string("File open error. Path: \"") + std::string(filePath) + "\""};
    }

    // If error with size occurs here, set COMPONENTS_READ_BUFFER_SIZE in CMake to 65535 or other size
    std::array<char, COMPONENTS_READ_BUFFER_SIZE> buffer;
    while (iFile) {
        const std::streamsize bytesRead = iFile.readsome(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        if ((bytesRead <= 0) ||
            !readCbk(
                std::string_view(buffer.data(), bytesRead),
                iFile.eof() || (bytesRead < COMPONENTS_READ_BUFFER_SIZE))) {
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
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject streamWriteFileData(const std::string_view &filePath, fileWriteCbk_t &&writeCbk, bool isSecureWrite)
{
    // Check file for validance
    if (stdfs::status(filePath).type() == stdfs::file_type::directory) {
        return {ExtraClasses::SystemFileError,
                std::string("Is directory. Path: \"") + std::string(filePath) + "\""};
    }
    if (!isWritable(filePath)) {
        return {ExtraClasses::SystemFileError,
                std::string("File not readable. Path: \"") + std::string(filePath) + "\""};
    }

    TemporaryFile oFile;
    oFile.setSecureMode(isSecureWrite);
    if (!oFile.setTargetFile(std::string(filePath), false)) {
        return {ExtraClasses::SystemFileError, "Failed to setup temporary file"};
    }

    // If error with size occurs here, set COMPONENTS_READ_BUFFER_SIZE in CMake to 8192 or other
    std::array<char, COMPONENTS_READ_BUFFER_SIZE> buffer;
    while (!oFile.eof()) {
        std::streamsize writableByteCount {};
        if (!writeCbk(buffer.data(), writableByteCount)) {
            break;
        }
        if (writableByteCount > COMPONENTS_READ_BUFFER_SIZE) {
            return {ExtraClasses::SystemFileError,
                    std::string("Invalid byte count to save. Path: \"") + std::string(filePath) + "\""};
        }
        oFile.write(buffer.data(), writableByteCount);
        oFile.flush();
    }
    if (oFile.bad()) {
        return {ExtraClasses::SystemFileError,
                std::string("Critical I/O error. Path: \"") + std::string(filePath) + "\""};
    }
    if (oFile.fail() && !oFile.eof()) {
        return {ExtraClasses::SystemFileError,
                std::string("Logical read error. Path: \"") + std::string(filePath) + "\""};
    }
    oFile.accept();
    return ExtraClasses::Ok;
}

ExtraClasses::ErrorObject streamTransformFileData(const std::string_view &filePath, fileTransformCbk_t &&transformCbk, bool isSecureWrite)
{
    // Check file for validance
    if (stdfs::status(filePath).type() == stdfs::file_type::directory) {
        return {ExtraClasses::SystemFileError,
                std::string("Is directory. Path: \"") + std::string(filePath) + "\""};
    }
    if (!isWritable(filePath)) {
        return {ExtraClasses::SystemFileError,
                std::string("File not readable. Path: \"") + std::string(filePath) + "\""};
    }

    TemporaryFile oFile;
    oFile.setSecureMode(isSecureWrite);
    if (!oFile.setTargetFile(std::string(filePath), false)) {
        return {ExtraClasses::SystemFileError, "Failed to setup temporary file"};
    }

    std::ifstream iFile;
    iFile.open(std::string(filePath), std::ios_base::in | std::ios_base::binary);
    if (!iFile.is_open()) {
        return {ExtraClasses::SystemFileError,
                std::string("Failed to open source file. Path: \"") + std::string(filePath) + "\""};
    }

    // If error with size occurs here, set COMPONENTS_READ_BUFFER_SIZE in CMake to 8192 or other
    std::array<char, COMPONENTS_READ_BUFFER_SIZE> sourceData;
    std::string replaceData;
    while (!oFile.eof()) {
        auto readByteCount = iFile.readsome(sourceData.data(), sourceData.size());
        if (readByteCount <= 0) {
            break;
        }

        replaceData.clear();
        if (!transformCbk(std::string_view(sourceData.data(), readByteCount),
                          replaceData,
                          oFile.eof() || (readByteCount < COMPONENTS_READ_BUFFER_SIZE))) {
            break;
        }
        if (replaceData.size() > static_cast<std::size_t>(COMPONENTS_READ_BUFFER_SIZE)) {
            return {ExtraClasses::SystemFileError,
                    std::string("Invalid byte count to save. Path: \"") + std::string(filePath) + "\""};
        }
        oFile.write(replaceData.data(), static_cast<std::streamsize>(replaceData.size()));
        oFile.flush();
    }
    if (iFile.bad() | oFile.bad()) {
        return {ExtraClasses::SystemFileError,
                std::string("Critical I/O error. Path: \"") + std::string(filePath) + "\""};
    }
    if (iFile.fail() && !iFile.eof()
        || oFile.fail() && !oFile.eof()) {
        return {ExtraClasses::SystemFileError,
                std::string("Logical read error. Path: \"") + std::string(filePath) + "\""};
    }
    oFile.accept();
    return ExtraClasses::Ok;
}

}
