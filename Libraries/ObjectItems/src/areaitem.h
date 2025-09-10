#ifndef AREAITEM_H
#define AREAITEM_H

#include "itembase.h"

namespace ObjectViewItems
{

class AreaItem : public ItemBase
{
public:
    AreaItem();

    void setRect(const QRectF& rect);

    void addToArea(ItemBase* pItem);
    void removeFromArea(ItemBase* pItem);

private:
    std::list<ItemBase*> m_subscribedItems;

protected:
    void processEvent(ItemBase* pSenderItem, EventType eventType) override;
};

}

#endif // AREAITEM_H
