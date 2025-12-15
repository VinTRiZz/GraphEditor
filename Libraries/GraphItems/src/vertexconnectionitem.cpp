#include "vertexconnectionitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/GraphObject.h>

#include "vertexitem.hpp"
#include "constants.hpp"

using namespace ObjectItems;

namespace Graph {

VertexConnectionItem::VertexConnectionItem(QGraphicsItem* parent)
    : BasicItem(parent) {
    setSystemName("Соединение вершин");
    setObjectType(OBJECTTYPE_CONNECTION);

    auto pLine = new ObjectItems::ArrowedConnectionLine;
    pLine->setDirection(LineDirectionType::Forward);
    setLineItem(pLine);

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

void VertexConnectionItem::setLineItem(ObjectItems::AbstractConnectionLine *pLine)
{
    if (pLine == nullptr) {
        throw std::invalid_argument("VertexConnectionItem: Nullptr line item");
    }
    delete m_connectionLine;
    m_connectionLine = pLine;
    pLine->setParentItem(this);
}

AbstractConnectionLine *VertexConnectionItem::getLineItem() const
{
    return m_connectionLine;
}

}  // namespace ObjectItems
