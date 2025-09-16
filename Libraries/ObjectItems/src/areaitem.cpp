#include "areaitem.h"

namespace ObjectViewItems
{

AreaItem::AreaItem()
{
    m_area = new QGraphicsPathItem(this);
    m_area->setFlag(ItemIsSelectable, false);
    registerSubitem(m_area);
}

void AreaItem::setBorderColor(const QColor &col)
{
    m_area->setPen(QPen(col, 2, Qt::SolidLine, Qt::RoundCap));
}

void AreaItem::setBackgroundColor(const QColor &col)
{
    m_area->setBrush(col);
}

void AreaItem::addToArea(ItemBase *pItem)
{
    pItem->subscribeForEvent(this, EventType::Move);
    auto itemPair = std::make_pair(pItem, new QGraphicsRectItem(this));

    itemPair.second->setPen(QPen(getBorderColor(), 2, Qt::SolidLine, Qt::RoundCap));
    itemPair.second->setBrush(getBackgroundColor());

    auto areaPath = m_area->path();
    if (areaPath.contains(pItem->pos()) &&
        areaPath.contains(pItem->pos() + pItem->boundingRect().topRight())) {
        return;
    }
    updateArea();
}

void AreaItem::removeFromArea(ItemBase *pItem)
{
    pItem->unsbscribeFromEvent(this, EventType::Move);

    auto targetItem = std::find(m_subscribedItems.begin(), m_subscribedItems.end(),
                               pItem);
    if (targetItem != m_subscribedItems.end()) {
        delete *targetItem;
        m_subscribedItems.erase(targetItem);
    }

    updateArea();
}

void AreaItem::updateArea()
{
    QPainterPath p;
    for (auto* pItem : m_subscribedItems) {
        if (p.contains(pItem->pos()) &&
            p.contains(pItem->pos() + pItem->boundingRect().topRight())) {
            break;
        }
        p.lineTo(pItem->boundingRect().center());
    }
    m_area->setPath(p);
}

void AreaItem::processEvent(ItemBase *pSenderItem, EventType eventType)
{
    auto areaPath = m_area->path();
    if (areaPath.contains(pSenderItem->pos()) &&
        areaPath.contains(pSenderItem->pos() + pSenderItem->boundingRect().topRight())) {
        return;
    }
    updateArea();
}

}
