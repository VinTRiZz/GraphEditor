#include "objectsinternalscene.h"

#include "objectsceneconstants.h"

#include <QGraphicsRectItem>

#include "logging.h"

using namespace ObjectViewItems;

ObjectsInternalScene::ObjectsInternalScene(QObject *parent) :
    QGraphicsScene(parent)
{

}

ObjectsInternalScene::~ObjectsInternalScene()
{

}

void ObjectsInternalScene::resizeScene(const QSize &iSize)
{
    m_pNullItem->setRectSize(QRectF(0, 0, iSize.width(), iSize.height()));
}

ObjectViewItems::ItemBase *ObjectsInternalScene::getParentOfComplex(QGraphicsItem *pItem)
{
    auto itemParentIdVariant = pItem->data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID);
    if (itemParentIdVariant.isNull()) {
        return dynamic_cast<ObjectViewItems::ItemBase*>(pItem);
    }
    return getObject(itemParentIdVariant.toLongLong());
}

bool ObjectsInternalScene::isNullItem(QGraphicsItem *pItem) const
{
    return (dynamic_cast<ObjectViewItems::DynamicAreaItem*>(pItem) != nullptr);
}

void ObjectsInternalScene::init()
{
    m_pNullItem = new DynamicAreaItem();
    m_pNullItem->setBrush(QColor(210, 215, 210));
    m_pNullItem->setPen(QPen(Qt::red, 2));
    m_pNullItem->setZValue(0);
    addItem(m_pNullItem);
}

void ObjectsInternalScene::clearScene()
{
    m_pNullItem->clearRegisteredItems();
}

void ObjectsInternalScene::addObject(ObjectViewItems::ItemBase *pItem)
{
    if (nullptr == pItem ||
        nullptr == dynamic_cast<ObjectViewItems::ItemBase*>(pItem)) {
        throw std::invalid_argument("ObjectsScene-internal: invalid (nullptr) item");
    }

    std::function<void(QGraphicsItem*, ObjectSceneConstants::objectId_t)> setChildComplexId =
        [&setChildComplexId](QGraphicsItem* pItem, ObjectSceneConstants::objectId_t parentId){
        pItem->setData(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID, parentId);
        for (auto* pChild : pItem->childItems()) {
            setChildComplexId(pChild, parentId);
        }
    };
    setChildComplexId(pItem, pItem->getObjectId());
    pItem->setData(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID, QVariant()); // Обнуление для сохранения зависимости parent-child
    m_objectsMap[pItem->getObjectId()] = pItem;
    m_pNullItem->registerItem(pItem);
}

ObjectViewItems::ItemBase *ObjectsInternalScene::getObject(ObjectSceneConstants::objectId_t objectId)
{
    auto targetObject = m_objectsMap.find(objectId);
    if (targetObject == m_objectsMap.end()) {
        return nullptr;
    }
    return targetObject.value();
}

QList<ObjectViewItems::ItemBase *> ObjectsInternalScene::getAllObjects() const
{
    return m_objectsMap.values();
}

QList<ObjectSceneConstants::objectId_t> ObjectsInternalScene::getAllObjectIds() const
{
    return m_objectsMap.keys();
}

void ObjectsInternalScene::removeObject(ObjectSceneConstants::objectId_t itemId)
{
    auto pItem = m_objectsMap.value(itemId, nullptr);
    if (pItem != nullptr) {
        m_objectsMap.remove(itemId);
        removeItem(pItem);
    }
}

void ObjectsInternalScene::setBackgroundColor(const QColor &bgrColor)
{
    m_pNullItem->setBrush(bgrColor);
}

void ObjectsInternalScene::setBorderColor(const QColor &borderColor)
{
    m_pNullItem->setPen(QPen(borderColor, 3));
}
