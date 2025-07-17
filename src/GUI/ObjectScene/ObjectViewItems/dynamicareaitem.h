#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include "itembase.h"

namespace ObjectViewItems
{

class DynamicAreaItem : public ItemBase
{
public:
    explicit DynamicAreaItem(QGraphicsItem* parent = nullptr);

    void setRectSize(const QRectF& iRect);
    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    std::list<ItemBase*> getRegisteredItems() const;
    void clearRegisteredItems();

private:
    QGraphicsRectItem* m_pCenterRect {nullptr};
    std::list<ItemBase*> m_registeredItems;
};

}

#endif // DYNAMICAREAITEM_H
