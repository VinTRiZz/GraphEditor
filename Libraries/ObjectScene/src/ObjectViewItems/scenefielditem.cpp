#include "scenefielditem.h"

#include <Common/Logging.h>

namespace ObjectViewItems
{

SceneFieldItem::SceneFieldItem(QGraphicsItem* parent) :
    ItemBase(parent)
{
    setSystemName("Dynamic area");
}

void SceneFieldItem::setRectSize(const QRectF &iRect)
{
    if (iRect.width() == 0 || iRect.height() == 0) {
        throw std::invalid_argument("Rect width or height is zero");
    }
    m_fieldRect = iRect;
}

void SceneFieldItem::setBrush(const QBrush &iBrush)
{
    m_fieldBrush = iBrush;
}

void SceneFieldItem::setPen(const QPen &iPen)
{
    m_gridPen = iPen;
}

void SceneFieldItem::registerItem(ItemBase *pItem)
{
    pItem->setParentItem(this);
    m_registeredItems.push_back(pItem);
}

std::list<ItemBase *> SceneFieldItem::getRegisteredItems() const
{
    return m_registeredItems;
}

void SceneFieldItem::clearRegisteredItems()
{
    for (auto pItem : m_registeredItems) {
        delete pItem;
    }
    m_registeredItems.clear();
}

void SceneFieldItem::removeRegisteredItems(ObjectViewConstants::ObjectType objT)
{
    auto removedBeginIt = std::remove_if(m_registeredItems.begin(), m_registeredItems.end(), [objT](auto* pItem){
        auto res = (pItem->getType() == objT);
        if (res) {
            delete pItem;
        }
        return res;
    });
    m_registeredItems.erase(removedBeginIt, m_registeredItems.end());
}

void SceneFieldItem::removeRegisteredItem(ItemBase *pItem)
{
    for (auto pRegItem : m_registeredItems) {
        if (pRegItem == pItem) {
            delete pRegItem;
        }
    }
    m_registeredItems.erase(std::find(m_registeredItems.begin(), m_registeredItems.end(), pItem));
}

bool SceneFieldItem::isIdAvailable(ObjectViewConstants::objectId_t itemId) const
{
    for (auto pItem : m_registeredItems) {
        if (pItem->getObjectId() == itemId) {
            return false;
        }
    }
    return true;
}

}
