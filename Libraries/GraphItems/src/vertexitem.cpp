#include "vertexitem.hpp"

#include "constants.hpp"

using namespace ObjectItems;

namespace Graph {

bool VertexItem::isLineSubscribed(VertexConnectionLine* pLine) {
    // Нет смысла проверять исходящие, т.к. нельзя регистрировать вершину саму
    // на себя
    for (auto pLineTo : m_connectionsToThis) {
        if (pLineTo->getVertexFrom()->getItemId() ==
            pLine->getVertexFrom()->getItemId()) {
            return true;
        }
    }
    return false;
}

VertexItem::VertexItem(QGraphicsItem *parent) :
    ObjectItems::BasicItem(parent)
{
    setObjectType(OBJECTTYPE_VERTEX);
    setSystemName("Vertex");
    createSubitem(m_nameItem);
    m_nameItem->setLineColor(Qt::black);
    m_nameItem->setZValue(100);

    setZValue(Layers::VERTEX_LAYER);

    connect(this, &BasicItem::displayNameChanged,
            m_nameItem, [this](){
        m_nameItem->setDisplayName(getDisplayName());
    });

    connect(this, &BasicItem::itemMoved,
            this, &VertexItem::updateConnectionLines);
}

void VertexItem::subscribeAsConnectionFrom(VertexConnectionLine* pLine) {
    if (this == pLine->getVertexTo()) {
        return;
    }

    if (nullptr != pLine->getVertexFrom()) {
        pLine->getVertexFrom()->unsubscribeConnectionFrom(pLine);
    }

    pLine->setVertexFrom(this);
    m_connectionsFromThis.emplace(pLine);
    updateConnectionLines();
}

void VertexItem::unsubscribeConnectionFrom(VertexConnectionLine* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexItem::subscribeAsConnectionTo(VertexConnectionLine* pLine) {
    if (this == pLine->getVertexFrom()) {
        return;
    }

    if (nullptr != pLine->getVertexTo()) {
        pLine->getVertexTo()->unsubscribeConnectionTo(pLine);
    }

    pLine->setVertexTo(this);
    m_connectionsToThis.emplace(pLine);
    updateConnectionLines();
}

void VertexItem::unsubscribeConnectionTo(VertexConnectionLine* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

void VertexItem::updateConnectionLines() {
    unsigned connectionNumber{0};
    auto vertexRadius = static_cast<double>(boundingRect().width()) / 2.0;

    for (auto pConFrom : m_connectionsFromThis) {
        auto fromPos =
            QPointF(x() + vertexRadius,
                    y() + 2 * vertexRadius + 20 + // TODO: Arrow size
                        m_nameItem->boundingRect().height() * 0.7);

        pConFrom->getLineItem()->setPositionFrom(fromPos);
        connectionNumber++;
    }

    connectionNumber = 0;
    for (auto pConTo : m_connectionsToThis) {
        auto toPos = QPointF(x() + vertexRadius, y() - 20); // TODO: Arrow size
        pConTo->getLineItem()->setPositionTo(toPos);
        connectionNumber++;
    }
}

TextLabel *VertexItem::getLabel() const
{
    return m_nameItem;
}

}
