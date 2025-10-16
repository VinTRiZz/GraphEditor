#include "connectionlineitem.h"

#include <AppInfrastructure/GraphEditorSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/Constants.h>

#include "vertexitembase.hpp"

using namespace ObjectViewItems;

namespace Graph {

VertexConnectionLine::VertexConnectionLine(QGraphicsItem* parent)
    : ItemBase(parent) {
    setSystemName("Соединение вершин");
    setType(ObjectViewItems::ObjectType(OBJECTTYPE_CONNECTION));

    m_connectionLine = new ElegantArrowLine(this);
    auto pLine = static_cast<ElegantArrowLine*>(m_connectionLine);
    registerSubitem(pLine);

    auto& appSettings = GraphEditorSettings::getInstance();

    pLine->setGradient1Color(appSettings.getObjectsConfig().m_defaultLineMainColor);
    pLine->setGradient2Color(appSettings.getObjectsConfig().m_defaultLineMainColor);
    pLine->setSelectionColor(appSettings.getObjectsConfig().m_defaultLineSelectionColor);
    pLine->setWeight(1);

    setZValue(Layers::CONNECTION_LAYER);
}

VertexConnectionLine::~VertexConnectionLine() {
    if (m_fromVertex) {
        m_fromVertex->unsubscribeConnectionFrom(this);
    }

    if (m_toVertex) {
        m_toVertex->unsubscribeConnectionTo(this);
    }
}

GConnection VertexConnectionLine::toConnection() const
{
    GConnection graphConnection;

    if (nullptr != getVertexFrom()) {
        graphConnection.idFrom = getVertexFrom()->getObjectId();
    }

    if (nullptr != getVertexTo()) {
        graphConnection.idTo = getVertexTo()->getObjectId();
    }

    graphConnection.name = getDisplayName();
    graphConnection.lineColor = m_connectionLine->getBorderColor();

    return graphConnection;
}

void VertexConnectionLine::fromConnection(const GConnection &con)
{
    setDisplayName(con.name);
    m_connectionLine->setBorderColor(con.lineColor);
}

void VertexConnectionLine::setVertexFrom(VertexItemBase* pVertexFrom) {
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

VertexItemBase* VertexConnectionLine::getVertexFrom() const {
    return m_fromVertex;
}

void VertexConnectionLine::setVertexTo(VertexItemBase* pVertexTo) {
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

VertexItemBase* VertexConnectionLine::getVertexTo() const {
    return m_toVertex;
}

void VertexConnectionLine::resetPositions() {
    if (nullptr == m_toVertex) {
        return;
    }
    m_toVertex->updateConnectionLines();
}

LineItem *VertexConnectionLine::getLineItem() const
{
    return m_connectionLine;
}

}  // namespace ObjectViewItems
