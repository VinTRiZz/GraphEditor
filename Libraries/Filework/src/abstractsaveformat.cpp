#include "abstractsaveformat.h"

namespace Filework
{

AbstractSaveFormat::AbstractSaveFormat()
{

}

AbstractSaveFormat::~AbstractSaveFormat()
{

}

void AbstractSaveFormat::setGraphMaintaner(Graph::PMaintainer pGraphMaintaner)
{
    m_pGraphMaintaner = pGraphMaintaner;
}

Graph::PMaintainer AbstractSaveFormat::getGraphMaintaner() const
{
    return m_pGraphMaintaner;
}

Graph::GraphObject& AbstractSaveFormat::getGraph() const
{
    if (!m_pGraphMaintaner) {
        throw std::runtime_error("SaveFormat: Did not set graph maintaner, requested graph (invalid operation)");
    }
    return m_pGraphMaintaner->getObject();
}

}
