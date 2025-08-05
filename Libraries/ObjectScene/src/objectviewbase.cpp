#include "objectviewbase.h"

#include <Common/Logging.h>

ObjectViewBase::ObjectViewBase(QWidget* parent) : QGraphicsView(parent) {
    m_pScene = new ObjectsInternalScene(this);
    setScene(m_pScene);

    m_pNullItem = new ObjectViewItems::SceneFieldItem();
    m_pNullItem->setBrush(QColor(220, 220, 220));
    m_pNullItem->setPen(QPen(QColor(70, 60, 60), 2));
    m_pNullItem->setZValue(-1);
    m_pScene->addItem(m_pNullItem);
}

bool ObjectViewBase::getIsInited() const {
    return (nullptr != m_pScene);
}

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

int ObjectViewBase::getGridSize() const {
    return m_pScene->getGridSize();
}

void ObjectViewBase::setSceneBrush(const QBrush& sceneBrush) {
    m_pScene->setBackgroundBrush(sceneBrush);
}

void ObjectViewBase::setCanvasBrush(const QBrush &canvasBrush)
{
    m_pNullItem->setBrush(canvasBrush);
}

void ObjectViewBase::setCanvasOpacity(double opac)
{
    m_pNullItem->setOpacity(opac);
}

void ObjectViewBase::setGridColor(const QColor &gColor)
{
    m_pScene->setGridPen({gColor, 2}); // TODO: Задание "жирности" сетки
}

void ObjectViewBase::setCanvasRect(const QRectF& iRect) {
    m_pNullItem->setFieldRect(iRect);

    auto targetWidth = m_pNullItem->boundingRect().width();
    targetWidth = targetWidth == 0 ? 1 : targetWidth;
    auto scaleCoeff = viewport()->width() / targetWidth;
    scale(scaleCoeff, scaleCoeff);

    auto rectCopy = iRect;
    rectCopy.moveTo(-10, -10);
    rectCopy.setSize(QSize(rectCopy.width(), rectCopy.height()));
    setSceneRect(rectCopy);
}

ObjectViewItems::ItemBase* ObjectViewBase::getParentOfComplex(
    QGraphicsItem* pItem) {
    auto itemParentIdVariant =
        pItem->data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID);
    if (itemParentIdVariant.isNull()) {
        return dynamic_cast<ObjectViewItems::ItemBase*>(pItem);
    }
    return getObject(itemParentIdVariant.toLongLong());
}

bool ObjectViewBase::isNullItem(QGraphicsItem* pItem) const {
    return (dynamic_cast<ObjectViewItems::SceneFieldItem*>(pItem) != nullptr);
}

void ObjectViewBase::removeSpecialObjects(
    ObjectViewConstants::ObjectType objT) {
    m_pNullItem->removeRegisteredItems(objT);
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

    // Закомментировал, т.к. это уже забота классов-наследников ItemBase
    //    std::function<void(QGraphicsItem*, ObjectViewConstants::objectId_t)>
    //    setChildComplexId =
    //        [&setChildComplexId](QGraphicsItem* pItem,
    //        ObjectViewConstants::objectId_t parentId){
    //        pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID,
    //        parentId); for (auto* pChild : pItem->childItems()) {
    //            setChildComplexId(pChild, parentId);
    //        }
    //    };
    //    setChildComplexId(pItem, pItem->getObjectId());
    //    pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID,
    //    QVariant()); // Обнуление для сохранения зависимости parent-child

    m_objectsMap[pItem->getObjectId()] = pItem;
    m_pNullItem->registerItem(pItem);
}

ObjectViewItems::ItemBase* ObjectViewBase::getObject(
    ObjectViewConstants::objectId_t objectId) const {
    auto targetObject = m_objectsMap.find(objectId);
    if (targetObject == m_objectsMap.end()) {
        return nullptr;
    }
    return targetObject.value();
}

QList<ObjectViewItems::ItemBase*> ObjectViewBase::getAllObjects() const {
    return m_objectsMap.values();
}

QList<ObjectViewConstants::objectId_t> ObjectViewBase::getAllObjectIds() const {
    return m_objectsMap.keys();
}

void ObjectViewBase::removeAllObjects() {}

void ObjectViewBase::removeObject(ObjectViewConstants::objectId_t itemId) {
    auto pItem = m_objectsMap.value(itemId, nullptr);
    if (pItem != nullptr) {
        m_objectsMap.remove(itemId);
        m_pNullItem->removeRegisteredItem(pItem);
    }
}
