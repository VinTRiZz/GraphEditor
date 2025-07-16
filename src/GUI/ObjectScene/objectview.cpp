#include "objectview.h"
#include "ui_objectview.h"
#include "objectsinternalscene.h"

#include <QWheelEvent>
#include <QMouseEvent>

#include <QScrollBar>

#include <QMenu>

#include <QGraphicsItem>
#include <QGraphicsRectItem>

#include "logging.h"

ObjectView::ObjectView(QWidget *parent) :
    QGraphicsView(parent),
    ui(new Ui::ObjectScene)
{
    ui->setupUi(this);
}

ObjectView::~ObjectView()
{
    delete ui;
}

void ObjectView::init()
{
    m_pScene = new ObjectsInternalScene(this);
    m_pScene->init();
    setScene(m_pScene);
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

ObjectSceneConstants::objectId_t ObjectView::addObject(QGraphicsItem *pItem)
{
    return m_pScene->addObject(pItem);
}

QList<ObjectSceneConstants::objectId_t> ObjectView::getAlObjectIds() const
{
    return m_pScene->getAlObjectIds();
}

void ObjectView::removeAllObjects()
{
    m_pScene->clearScene();

    // TODO: Убрать когда появится динамическое поле
    m_pScene->resizeScene(QSize(10000, 10000));
    scale(0.5, 0.5);
}

void ObjectView::removeObject(ObjectSceneConstants::objectId_t itemId)
{
    return m_pScene->removeObject(itemId);
}

QGraphicsItem *ObjectView::getGrabObject() const
{
    if (!m_grabObjectId.has_value()) {
        return nullptr;
    }
    return m_pScene->getObject(m_grabObjectId.value());
}

void ObjectView::setGrabObject(QGraphicsItem *pItem)
{
    if (nullptr == pItem) {
        m_grabObjectId = std::nullopt;
        return;
    }
    m_grabObjectId = pItem->data(ObjectSceneConstants::ObjectField::OBJECTFIELD_ID).toUInt();
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
    m_isHoldingLeftButton   = (e->button() == Qt::LeftButton);
    if (m_isHoldingLeftButton) {
        auto targetItem = itemAt(e->pos());
        if (!m_pScene->isNullItem(targetItem)) {
            targetItem = m_pScene->getParentOfComplex(targetItem);
            emit pressedOnItem(targetItem);
        }
    }

    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = mapToScene(e->pos());
    }
    QGraphicsView::mousePressEvent(e);
}

void ObjectView::mouseMoveEvent(QMouseEvent *e)
{
    auto currentPos = mapToScene(e->pos());
    if (m_grabObjectId.has_value()) {
        auto pObject = m_pScene->getObject(m_grabObjectId.value());
        pObject->setPos(currentPos - pObject->boundingRect().center());
    }

    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() - deltaPos.y());
    }
}

void ObjectView::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_isHoldingLeftButton) {
        auto targetItem = itemAt(e->pos());
        if (!m_pScene->isNullItem(targetItem)) {
            targetItem = m_pScene->getParentOfComplex(targetItem);
            emit releasedOnItem(targetItem);
            emit clickedOnItem(targetItem);
        }
    }

    if (m_isHoldingMiddleButton) {
        setCursor(Qt::ArrowCursor);
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
