#pragma once

#include <variant>
#include <string>

namespace Common
{

// Any value parsed in settings
using AppSettingValue_t =
    std::variant<
        std::monostate,
        std::string,
        int64_t,
        double>;


/**
 * @brief valueToString Converts value into string
 * @param val           Input value
 * @return              Result of conversion. If null or empty string, return ""
 */
inline std::string valueToString(const AppSettingValue_t& val) {
    if (std::holds_alternative<std::monostate>(val)) {
        return {};
    }
    if (std::holds_alternative<int64_t>(val)) {
        return std::to_string(std::get<int64_t>(val));
    }
    if (std::holds_alternative<double>(val)) {
        return std::to_string(std::get<double>(val));
    }
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val); // TODO: Use '\"' ?
    }
    return {};
}

}