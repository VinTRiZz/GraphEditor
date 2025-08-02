#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include "itembase.h"
#include "centeritem.h"

#include <QBrush>
#include <QPen>

namespace ObjectViewItems
{

class SceneFieldItem : public ItemBase
{
public:
    explicit SceneFieldItem(QGraphicsItem* parent = nullptr);

    void setFieldRect(const QRectF& iRect);
    QRectF getFieldRect() const;

    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    std::list<ItemBase*> getRegisteredItems() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectViewConstants::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);

    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

private:
    QGraphicsEllipseItem*   m_centerPoint {nullptr};
    QGraphicsRectItem*      m_centerRect {nullptr};
    std::list<ItemBase*>    m_registeredItems;
};

}

#endif // DYNAMICAREAITEM_H
