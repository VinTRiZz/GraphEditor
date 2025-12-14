#include "pluginobjectitnterface.hpp"

namespace Graph {

void PluginObjectInterface::setPluginName(const QString &pluginName)
{
    m_pluginName = pluginName;
}

QString PluginObjectInterface::getPluginName() const
{
    return m_pluginName;
}

void PluginObjectInterface::setPluginObjectName(const QString &pluginObjectName)
{
    m_pluginObjectName = pluginObjectName;
}

QString PluginObjectInterface::getPluginObjectName() const
{
    return m_pluginName;
}

} // namespace Graph
