#include "objectviewbase.h"

#include <Common/Logging.h>

#include <QScrollBar>

ObjectViewBase::ObjectViewBase(QWidget* parent) : QGraphicsView(parent) {
    m_pScene = new ObjectsInternalScene(this);
    setScene(m_pScene);

    m_pCanvasItem = new ObjectViewItems::SceneFieldItem();
    m_pCanvasItem->setBrush(QColor(220, 220, 220));
    m_pCanvasItem->setPen(QPen(QColor(70, 60, 60), 2));
    m_pCanvasItem->setZValue(-1);
    m_pScene->addItem(m_pCanvasItem);
}

bool ObjectViewBase::getIsInited() const {
    return (nullptr != m_pScene);
}

bool ObjectViewBase::isIdAvailable(
    ObjectViewConstants::objectId_t itemId) const {
    return m_pCanvasItem->isIdAvailable(itemId);
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

int ObjectViewBase::getGridSize() const {
    return m_pScene->getGridSize();
}

double ObjectViewBase::getGridLineWidth() const {
    return m_pScene->getGridLineWidth();
}

QRectF ObjectViewBase::getCanvasRect() const {
    return m_pCanvasItem->getFieldRect();
}

void ObjectViewBase::setSceneBrush(const QBrush& sceneBrush) {
    m_pScene->setBackgroundBrush(sceneBrush);
}

void ObjectViewBase::setCanvasBrush(const QBrush& canvasBrush) {
    m_pCanvasItem->setBrush(canvasBrush);
}

void ObjectViewBase::setCanvasOpacity(double opac) {
    m_pCanvasItem->setOpacity(opac);
}

void ObjectViewBase::setGridColor(const QColor& gColor) {
    m_pScene->setGridPen({gColor, 2});
}

void ObjectViewBase::setCanvasRect(const QRectF& iRect) {
    m_pCanvasItem->setFieldRect(iRect);

    resetTransform();

    QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();
    auto viewCenter = viewRect.center();

    auto deltaPos = iRect.center() - viewCenter;
    horizontalScrollBar()->setSliderPosition(
        horizontalScrollBar()->sliderPosition() + deltaPos.x());
    verticalScrollBar()->setSliderPosition(
        verticalScrollBar()->sliderPosition() + deltaPos.y());

    auto rectCopy = iRect;
    rectCopy.moveTo(-10, -10);
    rectCopy.setSize(QSize(rectCopy.width() + 10, rectCopy.height() + 10));
    setSceneRect(rectCopy);
}

ObjectViewItems::ItemBase* ObjectViewBase::getParentOfComplex(
    QGraphicsItem* pItem) {
    if (nullptr == pItem) [[unlikely]] {
        return nullptr;
    }
    auto itemParentIdVariant =
        pItem->data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID);
    if (itemParentIdVariant.isNull()) {
        return dynamic_cast<ObjectViewItems::ItemBase*>(pItem);
    }
    return getObject(itemParentIdVariant.toLongLong());
}

bool ObjectViewBase::isCanvasItem(QGraphicsItem* pItem) const {
    return (dynamic_cast<ObjectViewItems::SceneFieldItem*>(pItem) != nullptr);
}

ObjectViewItems::SceneFieldItem* ObjectViewBase::sceneCanvas() const {
    return m_pCanvasItem;
}

void ObjectViewBase::removeSpecialObjects(
    ObjectViewConstants::ObjectType objT) {
    m_pCanvasItem->removeRegisteredItems(objT);
}

ObjectsInternalScene* ObjectViewBase::scene() const {
    return m_pScene;
}

void ObjectViewBase::addObject(ObjectViewItems::ItemBase* pItem) {
    if (nullptr == pItem ||
        nullptr == dynamic_cast<ObjectViewItems::ItemBase*>(pItem)) {
        throw std::invalid_argument(
            "ObjectsScene-internal: invalid (nullptr) item");
    }
    m_pCanvasItem->registerItem(pItem);
}

ObjectViewItems::ItemBase* ObjectViewBase::getObject(
    ObjectViewConstants::objectId_t objectId) const {
    auto targetObject = m_pCanvasItem->getItem(objectId);
    if (targetObject == nullptr) {
        return nullptr;
    }
    return targetObject;
}

std::list<ObjectViewItems::ItemBase*> ObjectViewBase::getAllObjects() const {
    return m_pCanvasItem->getRegisteredItems();
}

std::list<ObjectViewConstants::objectId_t> ObjectViewBase::getAllObjectIds()
    const {
    return m_pCanvasItem->getRegisteredIds();
}

void ObjectViewBase::removeAllObjects() {}

void ObjectViewBase::removeObject(ObjectViewConstants::objectId_t itemId) {
    m_pCanvasItem->removeRegisteredItemById(itemId);
}
