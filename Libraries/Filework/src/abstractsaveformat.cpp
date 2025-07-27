#include "abstractsaveformat.h"

namespace Filework
{

AbstractSaveFormat::AbstractSaveFormat()
{

}

AbstractSaveFormat::~AbstractSaveFormat()
{

}

void AbstractSaveFormat::setGraphMaintaner(std::shared_ptr<Graph::GraphMaintaner> pGraphMaintaner)
{
    m_pGraphMaintaner = pGraphMaintaner;
}

std::shared_ptr<Graph::GraphMaintaner> AbstractSaveFormat::getGraphMaintaner() const
{
    return m_pGraphMaintaner;
}

Graph::GraphObject *AbstractSaveFormat::getGraph() const
{
    if (!m_pGraphMaintaner) {
        return nullptr;
    }
    return m_pGraphMaintaner->getObject();
}

}
