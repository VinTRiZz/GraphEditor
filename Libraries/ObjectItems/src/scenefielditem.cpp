#include "scenefielditem.h"

#include <Common/Logging.h>

namespace ObjectViewItems {

SceneFieldItem::SceneFieldItem(QGraphicsItem *parent) : ItemBase(parent) {
  setSystemName("Плоскость сцены");
  setSystemId();

  m_canvasRect = new QGraphicsRectItem(this);
  registerSubitem(m_canvasRect);
  m_canvasRect->setZValue(-10);
}

void SceneFieldItem::setFieldRect(const QRectF &iRect) {
  m_canvasRect->setRect(iRect);
}

QRectF SceneFieldItem::getFieldRect() const { return m_canvasRect->rect(); }

void SceneFieldItem::setBrush(const QBrush &iBrush) {
  m_canvasRect->setBrush(iBrush);
}

void SceneFieldItem::setPen(const QPen &iPen) { m_canvasRect->setPen(iPen); }

void SceneFieldItem::registerItem(ItemBase *pItem) {
  pItem->setParentItem(this);
  m_registeredItems[pItem->getType()].push_back(pItem);
}

ItemBase *
SceneFieldItem::getItem(ObjectViewConstants::objectId_t targetItemId,
                        ObjectViewConstants::ObjectType targetItemType) const {
  if (targetItemType == ObjectViewConstants::OBJECTTYPE_NONE) {
    for (auto &[itemType, items] : m_registeredItems) {
      for (auto pItem : items) {
        if (pItem->getObjectId() == targetItemId) {
          return pItem;
        }
      }
    }
  } else if (m_registeredItems.count(targetItemType) != 0) {
    for (auto pItem : m_registeredItems.at(targetItemType)) {
      if (pItem->getObjectId() == targetItemId) {
        return pItem;
      }
    }
  }
  return nullptr;
}

std::list<ItemBase *> SceneFieldItem::getRegisteredItems() const {
  std::list<ItemBase *> res;
  for (auto &[itemType, items] : m_registeredItems) {
    std::copy(items.begin(), items.end(), std::back_inserter(res));
  }
  return res;
}

std::list<ObjectViewConstants::objectId_t>
SceneFieldItem::getRegisteredIds() const {
  std::list<ObjectViewConstants::objectId_t> res;
  for (auto &[itemType, items] : m_registeredItems) {
    std::transform(items.begin(), items.end(), std::back_inserter(res),
                   [](auto *pItem) { return pItem->getObjectId(); });
  }
  return res;
}

void SceneFieldItem::clearRegisteredItems() {
  for (auto &[itemType, items] : m_registeredItems) {
    for (auto pItem : items) {
      delete pItem;
    }
  }
  m_registeredItems.clear();
}

void SceneFieldItem::removeRegisteredItems(
    ObjectViewConstants::ObjectType objT) {
  if (m_registeredItems.count(objT) == 0) {
    return;
  }

  auto &targetList = m_registeredItems[objT];

  auto removedBeginIt =
      std::remove_if(targetList.begin(), targetList.end(), [objT](auto *pItem) {
        auto res = (pItem->getType() == objT);
        if (res) {
          delete pItem;
        }
        return res;
      });
  targetList.erase(removedBeginIt, targetList.end());
}

void SceneFieldItem::removeRegisteredItem(ItemBase *pItem) {
  if (m_registeredItems.count(pItem->getType()) == 0) {
    return;
  }
  auto &targetItems = m_registeredItems[pItem->getType()];
  for (auto pRegItem : targetItems) {
    if (pRegItem == pItem) {
      delete pRegItem;
      targetItems.erase(
          std::find(targetItems.begin(), targetItems.end(), pItem));
      break;
    }
  }
}

void SceneFieldItem::removeRegisteredItemById(
    ObjectViewConstants::objectId_t targetItemId) {
  for (auto &[itemType, items] : m_registeredItems) {
    for (auto pItem : items) {
      if (pItem->getObjectId() == targetItemId) {
        delete pItem;
        items.erase(std::find(items.begin(), items.end(), pItem));
        return;
      }
    }
  }
}

bool SceneFieldItem::isIdAvailable(
    ObjectViewConstants::objectId_t itemId) const {
  for (auto &[itemType, items] : m_registeredItems) {
    for (auto pItem : items) {
      if (pItem->getObjectId() == itemId) {
        return false;
      }
    }
  }
  return true;
}

} // namespace ObjectViewItems
