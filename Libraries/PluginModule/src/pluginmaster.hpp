#pragma once

#include <string>
#include <memory>

#include <boost/noncopyable.hpp>

#include "grapheditorplugin.hpp"

#include <QObject>
#include <QString>

namespace Graph {

class PluginMaster : public QObject
{
    PluginMaster();
    Q_OBJECT
public:
    ~PluginMaster();
    static PluginMaster& getInstance();

    void init(const std::string& pluginRoot);
    bool addPlugin(const std::string& pluginFile);

    std::list<std::string> getPluginNames() const;
    std::shared_ptr<GraphEditorPlugin> getPlugin(const std::string& pluginName) const;
    std::list<std::shared_ptr<GraphEditorPlugin> > getAllPlugins() const;

signals:
    void pluginListChanged();

    void pluginInited(const QString& pluginName);
    void pluginDeinited(const QString& pluginName);

private:
    std::list<std::shared_ptr<GraphEditorPlugin> > m_plugins;
};

}
