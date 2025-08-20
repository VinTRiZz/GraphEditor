#include "objectviewbase.h"

#include <Common/Logging.h>

#include <QScrollBar>

ObjectViewBase::ObjectViewBase(QWidget *parent) : QGraphicsView(parent) {
  m_pScene = new ObjectsInternalScene(this);
  setScene(m_pScene);

  m_pNullItem = new ObjectViewItems::SceneFieldItem();
  m_pNullItem->setBrush(QColor(220, 220, 220));
  m_pNullItem->setPen(QPen(QColor(70, 60, 60), 2));
  m_pNullItem->setZValue(-1);
  m_pScene->addItem(m_pNullItem);
}

bool ObjectViewBase::getIsInited() const { return (nullptr != m_pScene); }

bool ObjectViewBase::isIdAvailable(
    ObjectViewConstants::objectId_t itemId) const {
  return m_pNullItem->isIdAvailable(itemId);
}

void ObjectViewBase::setGridEnabled(bool isGEnabled) {
  m_pScene->setGridEnabled(isGEnabled);
}

bool ObjectViewBase::getIsGridEnabled() const {
  return m_pScene->getIsGridEnabled();
}

void ObjectViewBase::setGridSize(int gridSizePx) {
  m_pScene->setGridSize(gridSizePx);
}

void ObjectViewBase::setGridLineWidth(double gridLineWidthPx) {
  m_pScene->setGridLineWidth(gridLineWidthPx);
}

int ObjectViewBase::getGridSize() const { return m_pScene->getGridSize(); }

double ObjectViewBase::getGridLineWidth() const {
  return m_pScene->getGridLineWidth();
}

QRectF ObjectViewBase::getCanvasRect() const {
  return m_pNullItem->getFieldRect();
}

void ObjectViewBase::setSceneBrush(const QBrush &sceneBrush) {
  m_pScene->setBackgroundBrush(sceneBrush);
}

void ObjectViewBase::setCanvasBrush(const QBrush &canvasBrush) {
  m_pNullItem->setBrush(canvasBrush);
}

void ObjectViewBase::setCanvasOpacity(double opac) {
  m_pNullItem->setOpacity(opac);
}

void ObjectViewBase::setGridColor(const QColor &gColor) {
  m_pScene->setGridPen({gColor, 2});
}

void ObjectViewBase::setCanvasRect(const QRectF &iRect) {
  m_pNullItem->setFieldRect(iRect);

  resetTransform();

  QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();
  auto viewCenter = viewRect.center();

  auto deltaPos = iRect.center() - viewCenter;
  horizontalScrollBar()->setSliderPosition(
      horizontalScrollBar()->sliderPosition() + deltaPos.x());
  verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() +
                                         deltaPos.y());

  auto rectCopy = iRect;
  rectCopy.moveTo(-10, -10);
  rectCopy.setSize(QSize(rectCopy.width() + 10, rectCopy.height() + 10));
  setSceneRect(rectCopy);
}

ObjectViewItems::ItemBase *
ObjectViewBase::getParentOfComplex(QGraphicsItem *pItem) {
  auto itemParentIdVariant =
      pItem->data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID);
  if (itemParentIdVariant.isNull()) {
    return dynamic_cast<ObjectViewItems::ItemBase *>(pItem);
  }
  return getObject(itemParentIdVariant.toLongLong());
}

bool ObjectViewBase::isNullItem(QGraphicsItem *pItem) const {
  return (dynamic_cast<ObjectViewItems::SceneFieldItem *>(pItem) != nullptr);
}

void ObjectViewBase::removeSpecialObjects(
    ObjectViewConstants::ObjectType objT) {
  m_pNullItem->removeRegisteredItems(objT);
}

ObjectsInternalScene *ObjectViewBase::scene() const { return m_pScene; }

void ObjectViewBase::addObject(ObjectViewItems::ItemBase *pItem) {
  if (nullptr == pItem ||
      nullptr == dynamic_cast<ObjectViewItems::ItemBase *>(pItem)) {
    throw std::invalid_argument(
        "ObjectsScene-internal: invalid (nullptr) item");
  }
  m_pNullItem->registerItem(pItem);
}

ObjectViewItems::ItemBase *
ObjectViewBase::getObject(ObjectViewConstants::objectId_t objectId) const {
  auto targetObject = m_pNullItem->getItem(objectId);
  if (targetObject == nullptr) {
    return nullptr;
  }
  return targetObject;
}

std::list<ObjectViewItems::ItemBase *> ObjectViewBase::getAllObjects() const {
  return m_pNullItem->getRegisteredItems();
}

std::list<ObjectViewConstants::objectId_t>
ObjectViewBase::getAllObjectIds() const {
  return m_pNullItem->getRegisteredIds();
}

void ObjectViewBase::removeAllObjects() {}

void ObjectViewBase::removeObject(ObjectViewConstants::objectId_t itemId) {
  m_pNullItem->removeRegisteredItemById(itemId);
}
