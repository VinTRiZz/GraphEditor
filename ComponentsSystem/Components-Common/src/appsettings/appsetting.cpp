#include "appsetting.hpp"

namespace Common {

void AppSetting::setName(const std::string &name)
{
    m_name = name;
}

std::string_view AppSetting::getName() const
{
    return m_name;
}

void AppSetting::setDescription(const std::string &descr)
{
    m_description = descr;
}

std::string_view AppSetting::getDescription() const
{
    return m_description;
}

bool AppSetting::setValue(const AppSettingValue_t &v)
{
    m_value = v;
    return true;
}

bool AppSetting::isSet() const
{
    return !std::holds_alternative<std::monostate>(m_value) && (!getValueString().empty()); // Empty string also mean not set
}

std::string AppSetting::getValueString() const
{
    return valueToString(m_value);
}

} // namespace Common
