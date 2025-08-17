#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include <QBrush>
#include <QPen>

#include <map>

#include "itembase.h"

namespace ObjectViewItems {

class SceneFieldItem : public ItemBase {
public:
    explicit SceneFieldItem(QGraphicsItem* parent = nullptr);

    void setFieldRect(const QRectF& iRect);
    QRectF getFieldRect() const;

    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    ItemBase* getItem(ObjectViewConstants::objectId_t targetItemId, ObjectViewConstants::ObjectType targetItemType = ObjectViewConstants::ObjectType::OBJECTTYPE_NONE) const;
    std::list<ItemBase*> getRegisteredItems() const;
    std::list<ObjectViewConstants::objectId_t> getRegisteredIds() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectViewConstants::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);
    void removeRegisteredItemById(ObjectViewConstants::objectId_t targetItemId);

    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

private:
    QGraphicsRectItem* m_canvasRect{nullptr};
    std::map<ObjectViewConstants::ObjectType, std::list<ItemBase*> > m_registeredItems;
};

}  // namespace ObjectViewItems

#endif  // DYNAMICAREAITEM_H
