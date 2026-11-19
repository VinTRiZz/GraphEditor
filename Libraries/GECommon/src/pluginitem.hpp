#pragma once

#include "abstractitem.hpp"

namespace GraphEditor {

template <typename BaseT>
class PluginItem : public BaseT
{
public:
    template <typename T>
    void setPlugin(T&& name) & { m_plugin = name; }
    std::string_view getPlugin() const & noexcept { return m_plugin; }


private:
    std::string m_plugin;
    std::string m_categoryName;
    std::string m_name;
};

}