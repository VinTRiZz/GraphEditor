#pragma once

#include <string>
#include <optional>

#include "common.hpp"

namespace Encryption
{

std::optional<std::string> encodeBase64(const std::string& input);
std::optional<std::string> decodeBase64(const std::string& input);

// Does NOT append 0x to output
std::optional<std::string> encodeHex(const std::string& input, bool useUpperCase = true);
std::optional<std::string> decodeHex(const std::string& input, bool useUpperCase = true);

// Windows encoding issues
std::optional<std::string> cp1251_to_utf8(const std::string_view& input);
std::optional<std::string> utf8_to_cp1251(const std::string_view& input);

}
