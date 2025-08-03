#include "scenefielditem.h"

#include <Common/Logging.h>

namespace ObjectViewItems {

SceneFieldItem::SceneFieldItem(QGraphicsItem* parent) : ItemBase(parent) {
    setSystemName("Плоскость сцены");

    m_centerRect = new QGraphicsRectItem(this);
    registerSubitem(m_centerRect);
    m_centerRect->setZValue(-10);

    m_centerPoint = new QGraphicsEllipseItem(this);
    m_centerPoint->setData(ObjectViewConstants::OBJECTFIELD_NAME_SYSTEM,
                           "Центр сцены");
    m_centerPoint->setRect(QRectF(0, 0, 5, 5));
    m_centerPoint->setBrush(QColor(255, 0, 0, 170));
    m_centerPoint->setFlag(ItemIgnoresTransformations, true);
    m_centerPoint->setZValue(-9);
}

void SceneFieldItem::setFieldRect(const QRectF& iRect) {
    m_centerRect->setRect(iRect);
    m_centerPoint->setPos(iRect.center() -
                          m_centerPoint->boundingRect().center());
}

QRectF SceneFieldItem::getFieldRect() const {
    return m_centerRect->rect();
}

void SceneFieldItem::setBrush(const QBrush& iBrush) {
    m_centerRect->setBrush(iBrush);
}

void SceneFieldItem::setPen(const QPen& iPen) {
    m_centerRect->setPen(iPen);
}

void SceneFieldItem::registerItem(ItemBase* pItem) {
    pItem->setParentItem(this);
    m_registeredItems.push_back(pItem);
}

std::list<ItemBase*> SceneFieldItem::getRegisteredItems() const {
    return m_registeredItems;
}

void SceneFieldItem::clearRegisteredItems() {
    for (auto pItem : m_registeredItems) {
        delete pItem;
    }
    m_registeredItems.clear();
}

void SceneFieldItem::removeRegisteredItems(
    ObjectViewConstants::ObjectType objT) {
    auto removedBeginIt =
        std::remove_if(m_registeredItems.begin(), m_registeredItems.end(),
                       [objT](auto* pItem) {
                           auto res = (pItem->getType() == objT);
                           if (res) {
                               delete pItem;
                           }
                           return res;
                       });
    m_registeredItems.erase(removedBeginIt, m_registeredItems.end());
}

void SceneFieldItem::removeRegisteredItem(ItemBase* pItem) {
    for (auto pRegItem : m_registeredItems) {
        if (pRegItem == pItem) {
            delete pRegItem;
        }
    }
    m_registeredItems.erase(
        std::find(m_registeredItems.begin(), m_registeredItems.end(), pItem));
}

bool SceneFieldItem::isIdAvailable(
    ObjectViewConstants::objectId_t itemId) const {
    for (auto pItem : m_registeredItems) {
        if (pItem->getObjectId() == itemId) {
            return false;
        }
    }
    return true;
}

}  // namespace ObjectViewItems
