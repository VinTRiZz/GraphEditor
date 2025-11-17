#include "vertexplugin.hpp"

#include "pluginlibrarymanager.hpp"

namespace Graph {

VertexPlugin::VertexPlugin()
{

}

VertexItem *VertexPlugin::getVertex(const std::string &vertexType) const
{
    auto createFunc = getLibraryManager()->getFunction<VertexItem*(const std::string&)>("createVertex");
    return createFunc(vertexType);
}

}
