#pragma once

#include <GraphItems/VertexObjectItem.h>

namespace Graph {

class VertexPlugin
{
    struct Impl;
    std::shared_ptr<Impl> d;

public:
    explicit VertexPlugin(const std::string pluginPath);

    static std::string getPluginExtension() { return "vpl"; }

    bool isLoaded() const;

    Graph::VertexItem* getVertex(const std::string& plugin, const std::string& vertexType);
};

}
