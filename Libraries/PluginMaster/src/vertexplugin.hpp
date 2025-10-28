#pragma once

#include <GraphItems/VertexObjectItem.h>

#include <memory>
#include <string>

namespace Graph {

class VertexPlugin
{
    struct Impl;
    std::shared_ptr<Impl> d;

    std::string m_pluginName;

public:
    explicit VertexPlugin(const std::string pluginPath);

    bool isLoaded() const;

    Graph::VertexItem* getVertex(const std::string& plugin, const std::string& vertexType);
};

}
