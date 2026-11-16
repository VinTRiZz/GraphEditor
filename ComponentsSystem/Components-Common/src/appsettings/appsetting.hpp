#pragma once

#include "appsettingscommon.hpp"

#include <string>

namespace Common {

/**
 * @brief The AppSetting class Basic value in settings
 */
class AppSetting
{
public:
    void setName(const std::string& name);
    std::string_view getName() const;

    void setDescription(const std::string& descr);
    std::string_view getDescription() const;

    virtual bool setValue(const AppSettingValue_t& v);
    bool isSet() const;

    template <typename T>
    T getValue() const {
        return std::get<T>(m_value);
    }
    std::string getValueString() const;

private:
    std::string m_name;
    std::string m_description;

    AppSettingValue_t m_value;
};

} // namespace Common
