#include "gluefilemetadata.hpp"

#include "hash.hpp"

#include <algorithm>
#include <filesystem>

namespace Encryption {

bool GlueFileMetadata::isValid() const
{
    return (fileSize != 0) && !fileHash.empty();
}

GlueFileMetadata GlueFileMetadata::fromFile(const std::string &iFile) {
    if (!std::filesystem::exists(iFile)) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Init glue file metadata",
                           "Invalid file (not exist)");
        return {};
    }
    GlueFileMetadata res;
    res.fileSize = std::filesystem::file_size(iFile);
    res.fileHash = sha256file(iFile).value();
    if (res.fileHash.empty()) {
        return {};
    }
    return res;
}

std::string GlueFileMetadata::toString() const {
    std::string metadata;
    const auto fileSizeSerialized = serializeValue(fileSize);
    std::copy_n(fileSizeSerialized.data(), fileSizeSerialized.size(), std::back_inserter(metadata));
    metadata += fileHash;

    const auto metadataHash = sha256(metadata).value(); // Must NOT work without this data
    metadata += metadataHash;

    if (metadata.size() != GLUE_METADATA_SIZE) {
        setEncryptionError(ExtraClasses::LogicalUnknown,
                           "Create metadata string",
                           "Invalid result size (internal error)");
        return {};
    }
    return metadata;
}

bool GlueFileMetadata::initFromString(const std::string &fileMetadata) {
    if (fileMetadata.size() != GLUE_METADATA_SIZE) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Read file metadata",
                           "Invalid metadata (too short)");
        return false;
    }

    auto metadata = std::string(fileMetadata.data(), GLUE_METADATA_SIZE - 64);
    const auto expectedHash = std::string(fileMetadata.data() + GLUE_METADATA_SIZE - 64, 64);
    if (expectedHash != sha256(metadata)) {
        setEncryptionError(ExtraClasses::SystemFileError,
                           "Read file metadata",
                           "Invalid metadata (broken)");
        return false;
    }

    std::array<uint8_t, 8> sizeSerialized {};
    std::copy_n(metadata.begin(), 8, sizeSerialized.begin());
    fileSize = deserializeValue(sizeSerialized);
    fileHash = std::string(metadata.data() + 8, 64);
    return true;
}

uint64_t GlueFileMetadata::deserializeValue(const std::array<uint8_t, 8> &bytes) {
    return static_cast<uint64_t>(bytes[0]) |
           (static_cast<uint64_t>(bytes[1]) << 8) |
           (static_cast<uint64_t>(bytes[2]) << 16) |
           (static_cast<uint64_t>(bytes[3]) << 24) |
           (static_cast<uint64_t>(bytes[4]) << 32) |
           (static_cast<uint64_t>(bytes[5]) << 40) |
           (static_cast<uint64_t>(bytes[6]) << 48) |
           (static_cast<uint64_t>(bytes[7]) << 56);
}

std::array<uint8_t, 8> GlueFileMetadata::serializeValue(const uint64_t value) {
    return {
        static_cast<uint8_t>(value),
        static_cast<uint8_t>(value >> 8),
        static_cast<uint8_t>(value >> 16),
        static_cast<uint8_t>(value >> 24),
        static_cast<uint8_t>(value >> 32),
        static_cast<uint8_t>(value >> 40),
        static_cast<uint8_t>(value >> 48),
        static_cast<uint8_t>(value >> 56)
    };
}

}