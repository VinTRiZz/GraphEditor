#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include "itembase.h"

#include <QBrush>
#include <QPen>

namespace ObjectViewItems
{

class SceneFieldItem : public ItemBase
{
public:
    explicit SceneFieldItem(QGraphicsItem* parent = nullptr);

    void setRectSize(const QRectF& iRect);
    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    std::list<ItemBase*> getRegisteredItems() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectViewConstants::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);

    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

private:
    QRectF m_fieldRect;
    QBrush m_fieldBrush;
    QPen   m_gridPen;
    std::list<ItemBase*> m_registeredItems;
};

}

#endif // DYNAMICAREAITEM_H
