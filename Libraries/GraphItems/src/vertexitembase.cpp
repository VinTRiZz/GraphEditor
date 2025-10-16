#include "vertexitembase.hpp"

using namespace ObjectViewItems;

namespace Graph {

bool VertexItemBase::isLineSubscribed(VertexConnectionLine* pLine) {
    // Нет смысла проверять исходящие, т.к. нельзя регистрировать вершину саму
    // на себя
    for (auto pLineTo : m_connectionsToThis) {
        if (pLineTo->getVertexFrom()->getObjectId() ==
            pLine->getVertexFrom()->getObjectId()) {
            return true;
        }
    }
    return false;
}

VertexItemBase::VertexItemBase(QGraphicsItem *parent) :
    GraphSceneItem(parent)
{
    m_nameItem = new LabelItem(this);
    registerSubitem(m_nameItem);
    m_nameItem->setBorderColor(Qt::black);
    m_nameItem->setZValue(0);
}

void VertexItemBase::setDisplayName(const QString &iText) {
    m_nameItem->setDisplayName(iText);
    ItemBase::setDisplayName(iText);

    // TODO: Поставить лейблу куда надо
}

void VertexItemBase::subscribeAsConnectionFrom(VertexConnectionLine* pLine) {
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

void VertexItemBase::unsubscribeConnectionFrom(VertexConnectionLine* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexItemBase::subscribeAsConnectionTo(VertexConnectionLine* pLine) {
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

void VertexItemBase::unsubscribeConnectionTo(VertexConnectionLine* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

void VertexItemBase::updateConnectionLines() {
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

QPainterPath VertexItemBase::shape() const {
    return m_nameItem->shape();
}

QVariant VertexItemBase::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        updateConnectionLines();
    }

    return ItemBase::itemChange(change, value);
}


}
