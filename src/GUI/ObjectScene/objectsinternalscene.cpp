#include "objectsinternalscene.h"
#include "objectsceneconstants.h"

#include <QGraphicsItem>
#include <QGraphicsRectItem>

ObjectsInternalScene::ObjectsInternalScene(QObject *parent) :
    QGraphicsScene(parent)
{
    uint currentId {1};
    m_idGenerator = [currentId]() mutable {
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

void ObjectsInternalScene::setIdGenerator(const std::function<uint ()> fGen)
{
    if (!fGen) {
        throw std::invalid_argument("ObjectScene-internal: invalid id generator passed into setIdGenerator");
    }

    m_idGenerator = fGen;
}

std::function<uint ()> ObjectsInternalScene::getIdGenerator() const
{
    return m_idGenerator;
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
    m_pNullItem = pNullRectItem;

    addItem(m_pNullItem);
}

void ObjectsInternalScene::clearScene()
{
    for (auto pChild : m_pNullItem->childItems()) {
        removeItem(pChild);
    }
    m_pNullItem->childItems().clear();
}

uint ObjectsInternalScene::addObject(QGraphicsItem *pItem)
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
    m_objectsMap[nextId] = pItem;
    pItem->setParentItem(m_pNullItem);
    return nextId;
}

QGraphicsItem *ObjectsInternalScene::getObject(uint objectId)
{
    auto targetObject = m_objectsMap.find(objectId);
    if (targetObject == m_objectsMap.end()) {
        return nullptr;
    }
    return targetObject.value();
}

QList<uint> ObjectsInternalScene::getAlObjectIds() const
{
    return m_objectsMap.keys();
}

void ObjectsInternalScene::removeObject(uint itemId)
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
