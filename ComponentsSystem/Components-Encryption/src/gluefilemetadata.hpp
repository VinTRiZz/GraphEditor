#pragma once

#include <string>
#include <array>
#include <stdint.h>

namespace Encryption {

inline const auto GLUE_METADATA_SIZE {136};
struct GlueFileMetadata
{
    std::size_t fileSize {};
    std::string fileHash {};

    bool isValid() const;

    static GlueFileMetadata fromFile(const std::string& iFile);
    std::string toString() const;
    bool initFromString(const std::string& fileMetadata);

    static std::array<uint8_t, 8> serializeValue(const uint64_t value);
    static uint64_t deserializeValue(const std::array<uint8_t, 8>& bytes);
};


}