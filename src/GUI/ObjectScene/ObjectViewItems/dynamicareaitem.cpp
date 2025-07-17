#include "dynamicareaitem.h"

#include "logging.h"

namespace ObjectViewItems
{

DynamicAreaItem::DynamicAreaItem(QGraphicsItem* parent) :
    ItemBase(parent)
{
    setName("Dynamic area");

    m_pCenterRect = new QGraphicsRectItem(this);

    QRect nullRect;
    nullRect.setWidth(500);
    nullRect.setHeight(500);

    m_pCenterRect->setZValue(0);
    m_pCenterRect->show();
}

void DynamicAreaItem::setRectSize(const QRectF &iRect)
{
    m_pCenterRect->setRect(iRect);
}

void DynamicAreaItem::setBrush(const QBrush &iBrush)
{
    m_pCenterRect->setBrush(iBrush);
}

void DynamicAreaItem::setPen(const QPen &iPen)
{
    m_pCenterRect->setPen(iPen);
}

void DynamicAreaItem::registerItem(ItemBase *pItem)
{
    pItem->setParentItem(m_pCenterRect);
    m_registeredItems.push_back(pItem);
}

std::list<ItemBase *> DynamicAreaItem::getRegisteredItems() const
{
    return m_registeredItems;
}

void DynamicAreaItem::clearRegisteredItems()
{
    for (auto pItem : m_registeredItems) {
        delete pItem;
    }
    m_registeredItems.clear();
}

}
