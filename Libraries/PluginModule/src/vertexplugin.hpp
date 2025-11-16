#pragma once

#include <GraphItems/SimpleVertexItem.h>

#include "abstractplugin.hpp"

namespace Graph {

class VertexPlugin : public AbstractPlugin
{
public:
    VertexPlugin();

    Graph::VertexItem* getVertex(const std::string& vertexType) const;
};

}
