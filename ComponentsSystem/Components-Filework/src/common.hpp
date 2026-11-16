#pragma once

#include <string>
#include <vector>
#include <functional>

#include <Components/ExtraClasses/Error.h>

namespace Filework::Common
{

// Used for outer usage
std::streamsize getBufferSize();

std::vector<std::string> getContentPaths(const std::string_view& dirPath);
std::vector<std::string> getContentNames(const std::string_view& dirPath, const std::string nameFilterRegexp = ".*");

bool isReadable(const std::string_view& objectPath);
bool isWritable(const std::string_view& objectPath);
ExtraClasses::ErrorObject isAccessable(const std::string_view& objectPath);
bool objectExist(const std::string_view& objectPath);
bool contains(const std::string_view& dirPath, const std::string_view& fileName);

int16_t objectCount(const std::string_view& dirPath);
int16_t fileCount(const std::string_view& dirPath);
int16_t dirCount(const std::string_view& dirPath);

ExtraClasses::ErrorObject readFileData(const std::string_view &filePath, std::string& output);
ExtraClasses::ErrorObject saveFileData(const std::string_view& filePath, const std::string_view& input);
ExtraClasses::ErrorObject appendFileData(const std::string_view& filePath, const std::string_view& input);


// Must return true if data reading accepted
// Arguments: readBuffer
using fileReadCbk_t = std::function<bool(const std::string_view&, bool)>;

/**
 * @brief streamReadFileData    Read file data block-by-block, according to COMPONENTS_READ_BUFFER_SIZE build define
 * @param filePath              Path to a file to read
 * @param readCbk               Callback to process read bytes
 * @return
 */
ExtraClasses::ErrorObject streamReadFileData(
    const std::string_view& filePath,
    fileReadCbk_t&& readCbk);

// Must return true while data writing not complete
// Arguments: writeBuffer, byteCount
using fileWriteCbk_t = std::function<bool(char*, std::streamsize&)>;

/**
 * @brief streamWriteFileData   Write file data block-by-block, according to COMPONENTS_READ_BUFFER_SIZE build define
 * @param filePath              Path to a file to create / replace data of
 * @param writeCbk              Callback to create data for writing
 * @param isSecureWrite         Security definer (according to TemporaryFile)
 * @return
 */
ExtraClasses::ErrorObject streamWriteFileData(
    const std::string_view& filePath,
    fileWriteCbk_t&& writeCbk,
    bool isSecureWrite = false);

// Must return true while data writing not complete
// Args: fileData, replaceBuffer, isLastSection
using fileTransformCbk_t = std::function<bool(const std::string_view&, std::string&, bool)>;

/**
 * @brief streamWriteFileData   Transform input file data block-by-block, according to COMPONENTS_READ_BUFFER_SIZE build define
 * @param filePath              Path to a file to create / replace data of
 * @param writeCbk              Callback to create data for writing
 * @param isSecureWrite         Security definer (according to TemporaryFile)
 * @return
 */
ExtraClasses::ErrorObject streamTransformFileData(
    const std::string_view& filePath,
    fileTransformCbk_t&& transformCbk,
    bool isSecureWrite = false);

}
