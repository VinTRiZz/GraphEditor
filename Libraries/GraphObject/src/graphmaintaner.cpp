#include "graphmaintaner.h"

namespace Graph
{

GraphMaintaner::GraphMaintaner()
{

}

GraphMaintaner::~GraphMaintaner()
{

}

std::shared_ptr<GraphMaintaner> GraphMaintaner::createInstance()
{
    return std::make_shared<GraphMaintaner>();
}

GraphObject *GraphMaintaner::getObject()
{
    return &m_graph;
}

GraphExtendedObject *GraphMaintaner::getExtendedObject()
{
    return &m_graph;
}

}
