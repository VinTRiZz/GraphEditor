#include "connectionlineitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>

#include "vertexitem.hpp"
#include "constants.hpp"

using namespace ObjectItems;

namespace Graph {

VertexConnectionLine::VertexConnectionLine(QGraphicsItem* parent)
    : BasicItem(parent) {
    setSystemName("Соединение вершин");
    setObjectType(OBJECTTYPE_CONNECTION);

    createSubitem(m_connectionLine);

    auto& appSettings = Common::ApplicationSettings::getInstance();

    m_connectionLine->setLineColor(Colors::DEFAULT_COLOR_CONNECTION_LINE);
    m_connectionLine->setSelectionColor(Colors::DEFAULT_COLOR_CONNECTION_SEL);
    m_connectionLine->setWidth(1);

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
        graphConnection.idFrom = getVertexFrom()->getItemId();
    }

    if (nullptr != getVertexTo()) {
        graphConnection.idTo = getVertexTo()->getItemId();
    }

    graphConnection.name = getDisplayName();
    graphConnection.color = m_connectionLine->getLineColor();

    return graphConnection;
}

void VertexConnectionLine::fromConnection(const GConnection &con)
{
    setDisplayName(con.name);
    m_connectionLine->setLineColor(con.color);
}

void VertexConnectionLine::setVertexFrom(VertexItem* pVertexFrom) {
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

VertexItem* VertexConnectionLine::getVertexFrom() const {
    return m_fromVertex;
}

void VertexConnectionLine::setVertexTo(VertexItem* pVertexTo) {
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

VertexItem* VertexConnectionLine::getVertexTo() const {
    return m_toVertex;
}

AbstractConnectionLine *VertexConnectionLine::getLineItem() const
{
    return m_connectionLine;
}

}  // namespace ObjectItems
