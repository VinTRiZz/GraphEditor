#pragma once

#include "abstractplugin.hpp"

#include <GraphItems/VertexConnectionItem.h>

namespace Graph {

class ConnectionPlugin : public AbstractPlugin
{
public:
    ConnectionPlugin();

    Graph::VertexConnectionLine* getConnection(const std::string& vertexType) const;
};

} // namespace Graph

