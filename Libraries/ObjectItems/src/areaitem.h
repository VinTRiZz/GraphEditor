#ifndef AREAITEM_H
#define AREAITEM_H

#include "itembase.h"

#include <boost/container/small_vector.hpp>

namespace ObjectViewItems
{

class AreaItem : public ItemBase
{
public:
    AreaItem();

    void setRect(const QRectF& rect);

    void setBorderColor(const QColor& col) override;
    void setBackgroundColor(const QColor& col) override;

    void addToArea(ItemBase* pItem);
    void removeFromArea(ItemBase* pItem);

private:
    std::list<ItemBase*> m_subscribedItems;
    QGraphicsPathItem* m_area {nullptr};

    void updateArea();

protected:
    void processEvent(ItemBase* pSenderItem, EventType eventType) override;
};

}

#endif // AREAITEM_H
