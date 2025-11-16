#pragma once

#include <string>
#include <memory>

#include <boost/noncopyable.hpp>

#include "vertexplugin.hpp"
#include "connectionplugin.hpp"

namespace Graph {

class PluginMaster : boost::noncopyable
{
    PluginMaster();
public:
    ~PluginMaster();
    static PluginMaster& getInstance();

    void init(const std::string& pluginRoot);
    std::list<std::string> getPluginNames() const;

    std::shared_ptr<AbstractPlugin> getPlugin(const std::string& pluginName) const;

    std::list<std::shared_ptr<ConnectionPlugin> > getConnecionPlugins() const;
    std::list<std::shared_ptr<VertexPlugin> > getVertexPlugins() const;

private:
    std::list<std::shared_ptr<AbstractPlugin> > m_plugins;
};

}
