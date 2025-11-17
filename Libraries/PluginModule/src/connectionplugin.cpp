#include "connectionplugin.hpp"

#include "pluginlibrarymanager.hpp"

namespace Graph {

ConnectionPlugin::ConnectionPlugin()
{

}

VertexConnectionLine *ConnectionPlugin::getConnection(const std::string &vertexType) const
{
    auto createFunc = getLibraryManager()->getFunction<VertexConnectionLine*(const std::string&)>("createConnection");
    return createFunc(vertexType);
}

} // namespace Graph
