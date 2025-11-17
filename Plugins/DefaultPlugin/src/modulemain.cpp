#include "modulemain.hpp"

#include <GraphItems/VertexItem.h>
#include <GraphItems/SimpleVertexItem.h>

const char *private_getLibraryItemNames()
{
    return "Aboba test";
}

const char *private_getItemType(const std::string &itemName)
{
    if (itemName == "Aboba test") {
        return "vertex";
    }
    return {};
}

Graph::VertexItem *private_createVertex(const std::string &name)
{
    return new Graph::SimpleVertexItem();
}

Graph::VertexConnectionLine *private_createConnection(const std::string &name)
{
    return nullptr;
}
