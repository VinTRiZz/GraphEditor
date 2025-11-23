#pragma once

#include <QString>

namespace Graph {

class PluginObjectInterface
{
public:
    void setPluginName(const QString& pluginName);
    QString getPluginName() const;

    void setPluginObjectName(const QString& pluginObjectName);
    QString getPluginObjectName() const;

private:
    QString m_pluginName;
    QString m_pluginObjectName;
};

} // namespace Graph

