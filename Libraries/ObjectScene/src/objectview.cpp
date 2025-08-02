#include "objectview.h"
#include "objectsinternalscene.h"

#include <QWheelEvent>
#include <QMouseEvent>

#include <QScrollBar>

#include <QMenu>

#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include <Common/Logging.h>

#include <math.h>

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
    m_pNullItem->setBrush(QColor(220, 220, 220));
    m_pNullItem->setPen(QPen(QColor(70, 60, 60), 2));
    m_pNullItem->setZValue(-1);
    m_pScene->addItem(m_pNullItem);

    m_pCoordinatesItem = new ObjectViewItems::LabelItem();
    m_pCoordinatesItem->setBackgroundColor(QColor(200, 240, 210, 80));
    m_pCoordinatesItem->setMainColor(Qt::black);
    m_pCoordinatesItem->setTextSize(10);
    m_pCoordinatesItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    m_pCoordinatesItem->setZValue(1e6); // Чтобы точно не закрыло
    m_pScene->addItem(m_pCoordinatesItem);

//    setSceneRect(QRectF(0, 0, 10e3, 10e3));
}

bool ObjectView::isInited() const
{
    return (nullptr != m_pScene);
}

void ObjectView::zoomIn()
{
    m_currentZoomValue *= 1.2;
    scale(1.2, 1.2);
}

void ObjectView::zoomOut()
{
    m_currentZoomValue *= 0.8;
    scale(0.8, 0.8);
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
        throw std::invalid_argument("Can not set grab object to nullptr");
        return;
    }
    m_grabObjectId = pItem->data(ObjectViewConstants::ObjectField::OBJECTFIELD_ID).toLongLong();
    LOG_DEBUG("Set grab object:", m_grabObjectId.value());
    m_grabObjectPos = pItem->pos();
}

void ObjectView::acceptGrabObject()
{
    centerOn(getObject(m_grabObjectId.value()));
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
    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    if (e->angleDelta().ry() > 0) {
        zoomOut();
    } else {
        zoomIn();
    }
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

    // Обновляем координаты и тип объекта
    {
        m_pCoordinatesItem->setPos(mapToScene({e->pos().x() + 20, e->pos().y() + 20}));

        QString hoverItemName {};
        auto hoverItem = itemAt(e->pos());
        if (nullptr != hoverItem) {
            auto pHoverItemParent = getParentOfComplex(hoverItem);
            if (nullptr != pHoverItemParent) {
                hoverItemName = pHoverItemParent->getSystemName();
            } else {
                hoverItemName = hoverItem->data(ObjectViewConstants::OBJECTFIELD_NAME_SYSTEM).toString();
            }
        }
        m_pCoordinatesItem->setShortName(
            QString("X: %0\nY: %1%2").arg(
                QString::number(currentPos.x()),
                QString::number(currentPos.y()),
                !hoverItemName.isEmpty() ? QString("\n") + hoverItemName : QString())
        );
    }

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

void ObjectView::setSceneBrush(const QBrush &sceneBrush)
{
    m_pScene->setBackgroundBrush(sceneBrush);
}

void ObjectView::setSceneRect(const QRectF &iRect)
{
    QGraphicsView::setSceneRect(iRect);
    setCanvasRect(m_pNullItem->getFieldRect());
}

void ObjectView::setCanvasRect(const QRectF &iRect)
{
    m_pNullItem->setFieldRect(iRect);
    m_pNullItem->setPos(sceneRect().center() - iRect.center());

    auto targetWidth = m_pNullItem->boundingRect().width();
    targetWidth = targetWidth == 0 ? 1 : targetWidth;
    auto scaleCoeff = viewport()->width() / targetWidth;
    scale(scaleCoeff, scaleCoeff);
    centerOn(sceneRect().center());
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

    // Закомментировал, т.к. это уже забота классов-наследников ItemBase
//    std::function<void(QGraphicsItem*, ObjectViewConstants::objectId_t)> setChildComplexId =
//        [&setChildComplexId](QGraphicsItem* pItem, ObjectViewConstants::objectId_t parentId){
//        pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID, parentId);
//        for (auto* pChild : pItem->childItems()) {
//            setChildComplexId(pChild, parentId);
//        }
//    };
//    setChildComplexId(pItem, pItem->getObjectId());
//    pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID, QVariant()); // Обнуление для сохранения зависимости parent-child

    LOG_DEBUG("Adding item with id:", pItem->getObjectId());
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
