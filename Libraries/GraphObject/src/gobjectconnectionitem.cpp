#include "gobjectconnectionitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/GraphObject.h>

#include "gobjectitem.hpp"

using namespace ObjectItems;

namespace Graph {

GObjectConnectionItem::GObjectConnectionItem(QGraphicsItem* parent)
    : BasicItem(parent) {
    setSystemName("Соединение вершин");
    setObjectType(OBJECTTYPE_CONNECTION);

    auto pLine = new ObjectItems::ArrowedConnectionLine;
    pLine->setDirection(LineDirectionType::Forward);
    setLineItem(pLine);

    auto& appSettings = Common::ApplicationSettings::getInstance();

    m_connectionLine->setLinePen(Colors::DEFAULT_COLOR_CONNECTION_LINE);
    m_connectionLine->setLineSelectionPen(Colors::DEFAULT_COLOR_CONNECTION_SEL);

    setZValue(Layers::CONNECTION_LAYER);
}

GObjectConnectionItem::~GObjectConnectionItem() {
    if (m_fromVertex) {
        m_fromVertex->unsubscribeConnectionFrom(this);
    }

    if (m_toVertex) {
        m_toVertex->unsubscribeConnectionTo(this);
    }
}

void GObjectConnectionItem::setVertexFrom(GObjectItem* pVertexFrom) {
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

GObjectItem* GObjectConnectionItem::getVertexFrom() const {
    return m_fromVertex;
}

void GObjectConnectionItem::setVertexTo(GObjectItem* pVertexTo) {
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

GObjectItem* GObjectConnectionItem::getVertexTo() const {
    return m_toVertex;
}

void GObjectConnectionItem::setLineItem(ObjectItems::AbstractConnectionLine *pLine)
{
    if (pLine == nullptr) {
        throw std::invalid_argument("VertexConnectionItem: Nullptr line item");
    }
    delete m_connectionLine;
    m_connectionLine = pLine;
    pLine->setParentItem(this);
}

AbstractConnectionLine *GObjectConnectionItem::getLineItem() const
{
    return m_connectionLine;
}

}  // namespace ObjectItems
