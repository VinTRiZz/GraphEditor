#include "pluginobjectitnterface.hpp"

namespace Graph {

void PluginObjectItnterface::setPluginName(const QString &pluginName)
{
    m_pluginName = pluginName;
}

QString PluginObjectItnterface::getPluginName() const
{
    return m_pluginName;
}

void PluginObjectItnterface::setPluginObjectName(const QString &pluginObjectName)
{
    m_pluginObjectName = pluginObjectName;
}

QString PluginObjectItnterface::getPluginObjectName() const
{
    return m_pluginName;
}

} // namespace Graph
