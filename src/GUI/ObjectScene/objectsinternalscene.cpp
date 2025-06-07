#include "objectsinternalscene.h"

#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include "objectsceneconstants.h"

#include "logging.h"

ObjectsInternalScene::ObjectsInternalScene(QObject *parent) :
    QGraphicsScene(parent)
{
    uint currentId {0};
    m_idGenerator = [currentId]() mutable {
        return currentId++;
    };
}

ObjectsInternalScene::~ObjectsInternalScene()
{

}

void ObjectsInternalScene::setIdGenerator(const std::function<uint ()> fGen)
{
    if (!fGen) {
        throw std::invalid_argument("ObjectScene: invalid id generator passed into setIdGenerator");
    }

    m_idGenerator = fGen;
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

uint ObjectsInternalScene::addObject(QGraphicsItem *pItem)
{
    auto nextId = m_idGenerator();

    // На всякий случай
    if (m_objectsMap.contains(nextId)) {
        throw std::runtime_error("ObjectScene-internal: ID, got from generator, already exists! Check generator functor");
    }

    m_objectsMap[nextId] = pItem;
    pItem->setParentItem(m_pNullItem);
    addItem(pItem);
    return nextId;
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
