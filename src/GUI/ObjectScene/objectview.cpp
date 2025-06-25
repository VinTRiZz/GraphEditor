#include "objectview.h"
#include "ui_objectview.h"
#include "objectsinternalscene.h"

#include <QWheelEvent>

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

QGraphicsItem *ObjectView::getContextMenuItem()
{
    return m_contextMenuItem;
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
    if (m_isHoldingLeftButton) {

        // TODO: REMOVE, IT'S TEST!
        auto pVertexContrastRect = new QGraphicsRectItem();
        QRect objrect;
        objrect.setWidth(150);
        objrect.setHeight(200);
        pVertexContrastRect->setRect(objrect);
        pVertexContrastRect->setPen(QPen(Qt::black, 3));
        pVertexContrastRect->setBrush(Qt::white);
        pVertexContrastRect->setZValue(100);
        pVertexContrastRect->setPos(e->scenePosition());

        setGrabObject(pVertexContrastRect);
    }

    m_isHoldingMiddleButton = (e->button() == Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = e->scenePosition();
    }
    QGraphicsView::mousePressEvent(e);
}

void ObjectView::mouseMoveEvent(QMouseEvent *e)
{
    auto currentPos = e->scenePosition();
    if (m_grabObjectId.has_value()) {
        auto pObject = m_pScene->getObject(m_grabObjectId.value());
        pObject->setPos(currentPos);
    }

    if (m_isHoldingMiddleButton) {
        horizontalScrollBar()->setSliderPosition(horizontalScrollBar()->sliderPosition() - (currentPos.x() - m_prevPos.x()));
        verticalScrollBar()->setSliderPosition(verticalScrollBar()->sliderPosition() - (currentPos.y() - m_prevPos.y()));
        m_prevPos = currentPos;
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
        m_contextMenuItem = itemAt(e->pos());
        m_pContextMenu->exec(e->globalPos());
        m_contextMenuItem = nullptr; // Обнуление во избежание проблем
    }
    QGraphicsView::contextMenuEvent(e);
}
