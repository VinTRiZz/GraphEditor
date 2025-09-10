#include "areaitem.h"

namespace ObjectViewItems
{

AreaItem::AreaItem()
{

}

void AreaItem::addToArea(ItemBase *pItem)
{
    pItem->subscribeForEvent(this, EventType::Move);
}

void AreaItem::removeFromArea(ItemBase *pItem)
{
    pItem->unsbscribeFromEvent(this, EventType::Move);
}

void AreaItem::processEvent(ItemBase *pSenderItem, EventType eventType)
{
    // TODO: Найти крайние айтемы

    // TODO: Выделить область между ними
}

}
