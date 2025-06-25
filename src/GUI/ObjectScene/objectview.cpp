#include "objectview.h"
#include "ui_objectview.h"
#include "objectsinternalscene.h"

#include <QWheelEvent>

#include <QScrollBar>

#include <QMenu>

#include <QGraphicsItem>

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

void ObjectView::resizeScene(const QSize &iSize)
{
    m_pScene->resizeScene(iSize);
}

void ObjectView::setIdGenerator(const std::function<uint ()> fGen)
{
    m_pScene->setIdGenerator(fGen);
}

std::function<uint ()> ObjectView::getIdGenerator() const
{
    return m_pScene->getIdGenerator();
}

void ObjectView::init()
{
    m_pScene = new ObjectsInternalScene(this);
    m_pScene->init();
    setScene(m_pScene);
}

void ObjectView::setContextMenu(QMenu *pMenu)
{
    m_pContextMenu = pMenu;
}

void ObjectView::clearScene()
{
    m_pScene->clearScene();
}

uint ObjectView::addObject(QGraphicsItem *pItem)
{
    return m_pScene->addObject(pItem);
}

void ObjectView::setGrabObject(QGraphicsItem *pItem)
{
    // Перезатираем предыдущий объект
    if (m_grabObjectId.has_value()) {
        m_pScene->removeObject(m_grabObjectId.value());
    }
    m_grabObjectId = m_pScene->addObject(pItem);
}

void ObjectView::acceptGrabObject()
{
    m_grabObjectId = std::nullopt;
}

void ObjectView::rejectGrabObject()
{
    if (m_grabObjectId.has_value()) {
        m_pScene->removeObject(m_grabObjectId.value());
    }
}

QList<uint> ObjectView::getAlObjectIds() const
{
    return m_pScene->getAlObjectIds();
}

void ObjectView::removeObject(uint itemId)
{
    return m_pScene->removeObject(itemId);
}

void ObjectView::wheelEvent(QWheelEvent *e)
{
    double scaleValue = e->angleDelta().ry() > 0 ? 0.8 : 1.2;

    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    scale(scaleValue, scaleValue);
    auto nextCursorPos = mapToGlobal(mapFromScene(cursorPos));
    QCursor::setPos(nextCursorPos);
}

void ObjectView::mousePressEvent(QMouseEvent *e)
{
    m_isHoldingLeftButton   = (e->button() == Qt::LeftButton);
    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);

    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = e->scenePosition();
    }
    QGraphicsView::mousePressEvent(e);
}

void ObjectView::mouseMoveEvent(QMouseEvent *e)
{
    if (m_grabObjectId.has_value()) {
        auto pObject = m_pScene->getObject(m_grabObjectId.value());
        pObject->setPos(e->scenePosition());
    }

    if (m_isHoldingMiddleButton) {
        auto prevPos = m_prevPos;
        auto currentPos = e->scenePosition();

        horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() - (currentPos.x() - prevPos.x()));
        verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() - (currentPos.y() - prevPos.y()));

        m_prevPos = e->scenePosition();
    }
}

void ObjectView::mouseReleaseEvent(QMouseEvent *e)
{
    m_isHoldingLeftButton = false;

    if (m_isHoldingMiddleButton) {
        setCursor(Qt::ArrowCursor);
    }
    m_isHoldingMiddleButton = false;

    QGraphicsView::mouseReleaseEvent(e);
}

void ObjectView::contextMenuEvent(QContextMenuEvent *e)
{
    if (nullptr != m_pContextMenu) {
        m_pContextMenu->exec(e->globalPos());
    }
    QGraphicsView::contextMenuEvent(e);
}
