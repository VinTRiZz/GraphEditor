#include "vertexconnectionitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>

#include "vertexitem.hpp"
#include "constants.hpp"

using namespace ObjectItems;

namespace Graph {

VertexConnectionItem::VertexConnectionItem(QGraphicsItem* parent)
    : BasicItem(parent) {
    setSystemName("Соединение вершин");
    setObjectType(OBJECTTYPE_CONNECTION);

    createSubitem(m_connectionLine);

    auto& appSettings = Common::ApplicationSettings::getInstance();

    m_connectionLine->setLinePen(Colors::DEFAULT_COLOR_CONNECTION_LINE);
    m_connectionLine->setSelectionPen(Colors::DEFAULT_COLOR_CONNECTION_SEL);

    setZValue(Layers::CONNECTION_LAYER);
}

VertexConnectionItem::~VertexConnectionItem() {
    if (m_fromVertex) {
        m_fromVertex->unsubscribeConnectionFrom(this);
    }

    if (m_toVertex) {
        m_toVertex->unsubscribeConnectionTo(this);
    }
}

GConnection VertexConnectionItem::toConnection() const
{
    GConnection graphConnection;

    if (nullptr != getVertexFrom()) {
        graphConnection.idFrom = getVertexFrom()->getItemId();
    }

    if (nullptr != getVertexTo()) {
        graphConnection.idTo = getVertexTo()->getItemId();
    }

    graphConnection.name = getDisplayName();
    graphConnection.color = m_connectionLine->getLinePen().color();

    return graphConnection;
}

void VertexConnectionItem::fromConnection(const GConnection &con)
{
    setDisplayName(con.name);
    m_connectionLine->setLinePen(con.color);
}

void VertexConnectionItem::setVertexFrom(VertexItem* pVertexFrom) {
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

VertexItem* VertexConnectionItem::getVertexFrom() const {
    return m_fromVertex;
}

void VertexConnectionItem::setVertexTo(VertexItem* pVertexTo) {
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

VertexItem* VertexConnectionItem::getVertexTo() const {
    return m_toVertex;
}

AbstractConnectionLine *VertexConnectionItem::getLineItem() const
{
    return m_connectionLine;
}

}  // namespace ObjectItems
