#include "objectsinternalscene.h"
#include "objectsceneconstants.h"

#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include "logging.h"

ObjectsInternalScene::ObjectsInternalScene(QObject *parent) :
    QGraphicsScene(parent)
{
    ObjectSceneConstants::objectId_t currentId {1};
    m_idGenerator = [currentId]() mutable -> ObjectSceneConstants::objectId_t {
        if (currentId + 1 == std::numeric_limits<decltype(currentId)>::max()) [[unlikely]] {
            return 1;
        }
        return currentId++;
    };
}

ObjectsInternalScene::~ObjectsInternalScene()
{

}

void ObjectsInternalScene::resizeScene(const QSize &iSize)
{
    static_cast<QGraphicsRectItem*>(m_pNullItem)->setRect(0, 0, iSize.width(), iSize.height());
}

QGraphicsItem *ObjectsInternalScene::getParentOfComplex(QGraphicsItem *pItem)
{
    auto itemParentIdVariant = pItem->data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID);
    if (itemParentIdVariant.isNull()) {
        return pItem;
    }
    return getObject(itemParentIdVariant.toUInt());
}

bool ObjectsInternalScene::isNullItem(QGraphicsItem *pItem) const
{
    return (pItem == m_pNullItem);
}

void ObjectsInternalScene::init()
{
    auto pNullRectItem = new QGraphicsRectItem();

    QRect nullRect;
    nullRect.setWidth(500);
    nullRect.setHeight(500);
    pNullRectItem->setRect(nullRect);
    pNullRectItem->setBrush(Qt::lightGray);
    pNullRectItem->setPen(QPen(Qt::red, 2));
    pNullRectItem->setZValue(0);

    m_pNullItem = pNullRectItem;
    pNullRectItem->setData(ObjectSceneConstants::OBJECTFIELD_ID, 0);
    m_pNullItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME, "NULL ITEM");

    addItem(m_pNullItem);
}

void ObjectsInternalScene::clearScene()
{
    for (auto pChild : m_pNullItem->childItems()) {
        removeItem(pChild);
    }
    m_pNullItem->childItems().clear();
}

ObjectSceneConstants::objectId_t ObjectsInternalScene::addObject(QGraphicsItem *pItem)
{
    if (pItem == nullptr) {
        throw std::invalid_argument("ObjectsScene-internal: invalid (nullptr) item");
    }
    auto nextId = m_idGenerator();

    // На всякий случай
    if (m_objectsMap.contains(nextId)) {
        throw std::runtime_error("ObjectScene-internal: ID, got from generator, already exists! Check generator functor");
    }

    pItem->setData(ObjectSceneConstants::OBJECTFIELD_ID, nextId);

    std::function<void(QGraphicsItem*, ObjectSceneConstants::objectId_t)> setChildComplexId = [&setChildComplexId](QGraphicsItem* pItem, ObjectSceneConstants::objectId_t parentId){
        pItem->setData(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID, parentId);
        for (auto* pChild : pItem->childItems()) {
            setChildComplexId(pChild, parentId);
        }
    };
    setChildComplexId(pItem, nextId);
    pItem->setData(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID, QVariant()); // Обнуление для сохранения зависимости parent-child

    m_objectsMap[nextId] = pItem;
    pItem->setParentItem(m_pNullItem);
    return nextId;
}

QGraphicsItem *ObjectsInternalScene::getObject(ObjectSceneConstants::objectId_t objectId)
{
    auto targetObject = m_objectsMap.find(objectId);
    if (targetObject == m_objectsMap.end()) {
        return nullptr;
    }
    return targetObject.value();
}

QList<ObjectSceneConstants::objectId_t> ObjectsInternalScene::getAlObjectIds() const
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
    static_cast<QGraphicsRectItem*>(m_pNullItem)->setBrush(bgrColor);
}

void ObjectsInternalScene::setBorderColor(const QColor &borderColor)
{
    static_cast<QGraphicsRectItem*>(m_pNullItem)->setPen(QPen(borderColor, 2));
}
