#include "abstractsaveformat.h"

namespace Filework
{

AbstractSaveFormat::AbstractSaveFormat()
{

}

AbstractSaveFormat::~AbstractSaveFormat()
{

}

void AbstractSaveFormat::setGraph(Graph::GraphObject *pGraph)
{
    m_pGraph = pGraph;
}

Graph::GraphObject *AbstractSaveFormat::getGraph() const
{
    return m_pGraph;
}

}
