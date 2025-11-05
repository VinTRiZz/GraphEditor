#include "maintaineruserdecorator.hpp"

namespace Graph {

void MaintainerUserDecorator::setGraph(const PMaintainer &pMaintainer)
{
    m_graphMaintainer = pMaintainer;
    processGraphChange();
}

PMaintainer MaintainerUserDecorator::getGraph() const
{
    return m_graphMaintainer;
}

bool MaintainerUserDecorator::isGraphSet() const
{
    return (nullptr != m_graphMaintainer);
}

} // namespace Graph
