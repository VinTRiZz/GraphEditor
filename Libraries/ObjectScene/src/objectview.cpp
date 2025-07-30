#include "objectview.h"
#include "objectsinternalscene.h"

#include <QWheelEvent>
#include <QMouseEvent>

#include <QScrollBar>

#include <QMenu>

#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include <Common/Logging.h>

using namespace ObjectViewItems;

ObjectView::ObjectView(QWidget *parent) :
    QGraphicsView(parent)
{

}

ObjectView::~ObjectView()
{

}

void ObjectView::init()
{
    m_pScene = new ObjectsInternalScene(this);
    setScene(m_pScene);

    m_pNullItem = new SceneFieldItem();
    m_pNullItem->setBrush(QColor(210, 215, 210));
    m_pNullItem->setPen(QPen(QColor(110, 115, 110), 2));
    m_pNullItem->setZValue(0);
    m_pScene->addItem(m_pNullItem);

    scale(0.5, 0.5);
}

bool ObjectView::isInited() const
{
    return (nullptr != m_pScene);
}

void ObjectView::setContextMenu(QMenu *pMenu)
{
    m_pContextMenu = pMenu;
}

QGraphicsItem *ObjectView::getContextMenuItem()
{
    return m_contextMenuItem;
}

QGraphicsItem *ObjectView::getGrabObject() const
{
    if (!m_grabObjectId.has_value()) {
        return nullptr;
    }
    return getObject(m_grabObjectId.value());
}

void ObjectView::setMovingCallback(const std::function<void (const QPointF &)> &callbackFunc)
{
    m_movingCallback = callbackFunc;
}

void ObjectView::setGrabObject(QGraphicsItem *pItem)
{
    if (nullptr == pItem) {
        m_grabObjectId = std::nullopt;
        return;
    }
    m_grabObjectId = pItem->data(ObjectViewConstants::ObjectField::OBJECTFIELD_ID).toLongLong();
    m_grabObjectPos = pItem->pos();
}

void ObjectView::acceptGrabObject()
{
    m_grabObjectId = std::nullopt;
}

void ObjectView::rejectGrabObject()
{
    if (!m_grabObjectId.has_value()) {
        throw std::runtime_error("ObjectView Error: can not reject without item grabbed");
    }
    auto grabObject = getGrabObject();
    grabObject->setPos(m_grabObjectPos);
    m_grabObjectId = std::nullopt;
}

void ObjectView::wheelEvent(QWheelEvent *e)
{
    double scaleValue = e->angleDelta().ry() > 0 ? 0.8 : 1.2;
    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    scale(scaleValue, scaleValue);
    centerOn(cursorPos);
    auto nextCursorPos = mapToGlobal(mapFromScene(cursorPos));
    QCursor::setPos(nextCursorPos);
}

void ObjectView::mousePressEvent(QMouseEvent *e)
{
    setCursor(Qt::ArrowCursor);

    m_isHoldingLeftButton   = (e->button() == Qt::LeftButton);
    if (m_isHoldingLeftButton) {
        auto targetItem = itemAt(e->pos());
        if (!isNullItem(targetItem)) {
            targetItem = getParentOfComplex(targetItem);
            emit pressedOnItem(targetItem);
        } else {
            emit pressedOnItem(nullptr);
        }
    }

    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = mapToScene(e->pos());
    }

    if (m_movingCallback) {
        m_movingCallback(mapToScene(e->pos()));
    }
    QGraphicsView::mousePressEvent(e);
}

void ObjectView::mouseMoveEvent(QMouseEvent *e)
{
    auto currentPos = mapToScene(e->pos());
    if (m_grabObjectId.has_value()) {
        auto pObject = getGrabObject();
        pObject->setPos(currentPos - pObject->boundingRect().center());
    }

    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() - deltaPos.y());
    }

    if (m_movingCallback) {
        m_movingCallback(mapToScene(e->pos()));
    }
}

void ObjectView::mouseReleaseEvent(QMouseEvent *e)
{
    setCursor(Qt::ArrowCursor);

    if (m_isHoldingLeftButton) {
        auto targetItem = itemAt(e->pos());
        if (!isNullItem(targetItem)) {
            targetItem = getParentOfComplex(targetItem);
            emit releasedOnItem(targetItem);
            emit clickedOnItem(targetItem);
        } else {
            emit releasedOnItem(nullptr);
        }
    }

    m_isHoldingLeftButton = false;
    m_isHoldingMiddleButton = false;
    QGraphicsView::mouseReleaseEvent(e);
}

void ObjectView::contextMenuEvent(QContextMenuEvent *e)
{
    if (nullptr != m_pContextMenu) {
        m_contextMenuItem = itemAt(e->pos());
        m_pContextMenu->exec(e->globalPos());
        m_contextMenuItem = nullptr; // Обнуление во избежание проблем
    }
    QGraphicsView::contextMenuEvent(e);
}



bool ObjectView::isIdAvailable(ObjectViewConstants::objectId_t itemId) const
{
    return m_pNullItem->isIdAvailable(itemId);
}

ObjectViewItems::ItemBase *ObjectView::getParentOfComplex(QGraphicsItem *pItem)
{
    auto itemParentIdVariant = pItem->data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID);
    if (itemParentIdVariant.isNull()) {
        return dynamic_cast<ObjectViewItems::ItemBase*>(pItem);
    }
    return getObject(itemParentIdVariant.toLongLong());
}

bool ObjectView::isNullItem(QGraphicsItem *pItem) const
{
    return (dynamic_cast<ObjectViewItems::SceneFieldItem*>(pItem) != nullptr);
}

void ObjectView::removeSpecialObjects(ObjectViewConstants::ObjectType objT)
{
    m_pNullItem->removeRegisteredItems(objT);
}

void ObjectView::addObject(ObjectViewItems::ItemBase *pItem)
{
    if (nullptr == pItem ||
        nullptr == dynamic_cast<ObjectViewItems::ItemBase*>(pItem)) {
        throw std::invalid_argument("ObjectsScene-internal: invalid (nullptr) item");
    }

    std::function<void(QGraphicsItem*, ObjectViewConstants::objectId_t)> setChildComplexId =
        [&setChildComplexId](QGraphicsItem* pItem, ObjectViewConstants::objectId_t parentId){
        pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID, parentId);
        for (auto* pChild : pItem->childItems()) {
            setChildComplexId(pChild, parentId);
        }
    };
    setChildComplexId(pItem, pItem->getObjectId());
    pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID, QVariant()); // Обнуление для сохранения зависимости parent-child
    m_objectsMap[pItem->getObjectId()] = pItem;
    m_pNullItem->registerItem(pItem);
}

ObjectViewItems::ItemBase *ObjectView::getObject(ObjectViewConstants::objectId_t objectId) const
{
    auto targetObject = m_objectsMap.find(objectId);
    if (targetObject == m_objectsMap.end()) {
        return nullptr;
    }
    return targetObject.value();
}

QList<ObjectViewItems::ItemBase *> ObjectView::getAllObjects() const
{
    return m_objectsMap.values();
}

QList<ObjectViewConstants::objectId_t> ObjectView::getAllObjectIds() const
{
    return m_objectsMap.keys();
}

void ObjectView::removeObject(ObjectViewConstants::objectId_t itemId)
{
    auto pItem = m_objectsMap.value(itemId, nullptr);
    if (pItem != nullptr) {
        m_objectsMap.remove(itemId);
        m_pNullItem->removeRegisteredItem(pItem);
    }
}
