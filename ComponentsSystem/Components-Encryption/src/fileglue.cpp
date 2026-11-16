#include "fileglue.hpp"

#include <fstream>
#include <filesystem>

#include <Components/Filework/TemporaryFile.h>

#include "gluefilemetadata.hpp"
#include "hash.hpp"

namespace Encryption {

static std::pair<GlueFileMetadata, GlueFileMetadata> readFilesMetadata(const std::string& inputFile) {
    if (!std::filesystem::exists(inputFile)) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Glued files metadata read",
                           "Glued file not exist");
        return {};
    }

    std::ifstream ifile(inputFile, std::ios_base::in | std::ios_base::binary);
    if (!ifile.is_open()) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Glued files metadata read",
                           "Failed to open glued file");
        return {};
    }

    char readBuffer[GLUE_METADATA_SIZE * 2];
    auto readBytes = ifile.readsome(readBuffer, GLUE_METADATA_SIZE * 2);
    if (GLUE_METADATA_SIZE * 2 != readBytes) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Read glued files metadata",
                           "Failed to read expected amount of bytes");
        return {};
    }

    std::pair<GlueFileMetadata, GlueFileMetadata> res;
    res.first.initFromString(std::string(readBuffer, GLUE_METADATA_SIZE));
    if (!res.first.isValid()) { return {}; }

    res.second.initFromString(std::string(readBuffer + GLUE_METADATA_SIZE, GLUE_METADATA_SIZE));
    if (!res.second.isValid()) { return {}; }

    return res;
}

bool glueFiles(const std::string &fileA, const std::string &fileB, const std::string &resultFile)
{
    if (!std::filesystem::exists(fileA) || !std::filesystem::exists(fileB)) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Glue files",
                           "Failed to open input files");
        return false;
    }

    // Get metadata of file A
    const auto metadataFileA = GlueFileMetadata::fromFile(fileA);
    if (!metadataFileA.isValid()) { return false; }
    const auto metadataFileA_serialized = metadataFileA.toString();
    if (metadataFileA_serialized.empty()) { return false; }

    // Get metadata of file B
    const auto metadataFileB = GlueFileMetadata::fromFile(fileB);
    if (!metadataFileB.isValid()) { return false; }
    const auto metadataFileB_serialized = metadataFileB.toString();
    if (metadataFileB_serialized.empty()) { return false; }

    // Save metadata
    Filework::TemporaryFile ofile(resultFile);
    ofile.write(metadataFileA_serialized.c_str(), metadataFileA_serialized.size());
    ofile.flush();
    ofile.write(metadataFileB_serialized.c_str(), metadataFileB_serialized.size());
    ofile.flush();

    // Save both files data
    auto appendFileData = [&ofile](const std::string& iFile) -> bool {
        std::ifstream ifile(iFile, std::ios_base::in | std::ios_base::binary);
        if (!ifile.is_open()) {
            setEncryptionError(ExtraClasses::SystemFileError,
                               "Read glued files",
                               "Failed to open input file A");
            return false;
        }
        const auto expectedSize = std::filesystem::file_size(iFile);
        std::size_t writtenSize {};

        const auto bufferSize = 4096;
        char readBuffer[bufferSize];
        auto readBytes = ifile.readsome(readBuffer, bufferSize);
        while (readBytes > 0) {
            ofile.write(readBuffer, readBytes);
            ofile.flush();
            writtenSize += readBytes;
            readBytes = ifile.readsome(readBuffer, bufferSize);
        }
        auto res = (expectedSize == writtenSize);
        if (ifile.bad() || !res) {
            setEncryptionError(ExtraClasses::SystemFileError,
                               "Read glued files",
                               std::string("Failed to read part of input file ") + iFile);
            return false;
        }
        return true;
    };
    auto isWritten = appendFileData(fileA);
    if (!isWritten) { return false; }

    isWritten = appendFileData(fileB);
    if (!isWritten) { return false; }

    ofile.accept();
    return true;
}

bool splitGluedFiles(const std::string &inputFile, const std::string& fileA, const std::string& fileB)
{
    if (!std::filesystem::exists(inputFile)) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Glued file read",
                           "Glued file not exist");
        return false;
    }

    const auto filesMetadata = readFilesMetadata(inputFile);
    if (!filesMetadata.first.isValid() || !filesMetadata.second.isValid()) {
        return false;
    }

    std::ifstream gluedFile(inputFile, std::ios_base::in | std::ios_base::binary);
    if (!gluedFile.is_open()) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Glued file read",
                           "Failed to open glued file");
        return false;
    }
    gluedFile.seekg(GLUE_METADATA_SIZE * 2, std::ios_base::beg);

    auto extractFileData = [&gluedFile](const std::string& iFile, const GlueFileMetadata& fileMetadata) -> bool {
        std::ofstream ofile(iFile, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if (!ofile.is_open()) {
            setEncryptionError(ExtraClasses::SystemFileError,
                               "Create glued file output",
                               std::string("Failed to glued source file ") + iFile);
            return false;
        }
        auto expectedSize = fileMetadata.fileSize;

        const uint64_t bufferSize = 4096;
        char readBuffer[bufferSize];
        auto readBytes = gluedFile.readsome(readBuffer, std::min(bufferSize, expectedSize));
        while (readBytes > 0) {
            expectedSize -= readBytes;
            ofile.write(readBuffer, readBytes);
            ofile.flush();
            readBytes = gluedFile.readsome(readBuffer, std::min(bufferSize, expectedSize));
        }
        if (expectedSize > 0) {
            setEncryptionError(ExtraClasses::SystemFileError,
                               "Create glued file output",
                               std::string("Failed to glued source file ") + iFile);
            return false;
        }
        return (fileMetadata.fileHash == sha256file(iFile));
    };
    auto isExtracted = extractFileData(fileA, filesMetadata.first);
    if (!isExtracted) { return false; }

    isExtracted = extractFileData(fileB, filesMetadata.second);
    if (!isExtracted) { return false; }

    return true;
}

}