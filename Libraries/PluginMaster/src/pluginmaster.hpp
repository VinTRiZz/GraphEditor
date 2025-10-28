#pragma once

#include <string>
#include <memory>

#include <boost/noncopyable.hpp>

#include "vertexplugin.hpp"

namespace Graph {

class PluginMaster : boost::noncopyable
{
    PluginMaster();
public:
    ~PluginMaster();
    static PluginMaster& getInstance();

    void init(const std::string& pluginRoot);

private:
    std::list<VertexPlugin> m_vertexPlugins;
};

}
