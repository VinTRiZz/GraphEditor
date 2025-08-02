#include "interactiveobjectview.h"

#include <Common/Logging.h>
#include <QWheelEvent>

#include <QMenu>
#include <QScrollBar>

InteractiveObjectView::InteractiveObjectView(QWidget *parent) :
    ObjectViewBase(parent)
{
    m_mainContextMenu = new QMenu(this);

    m_mainContextMenu->addAction("Переключить сетку", [this](){
        scene()->setGridEnabled(!scene()->getIsGridEnabled());
    });
}

void InteractiveObjectView::zoomIn()
{
    scale(1.2, 1.2);
    emit scaleChanged();
}

void InteractiveObjectView::zoomOut()
{
    scale(0.8, 0.8);
    emit scaleChanged();
}

void InteractiveObjectView::customZoom(double scaleCoeff)
{
    scale(scaleCoeff, scaleCoeff);
    emit scaleChanged();
}

double InteractiveObjectView::getCurrentScale() const {
    return transform().m11();
}

void InteractiveObjectView::setContextMenu(QMenu *pMenu)
{
    m_mainContextMenu->addMenu(pMenu);
}

QGraphicsItem *InteractiveObjectView::getContextMenuItem()
{
    return m_contextMenuItem;
}

QGraphicsItem *InteractiveObjectView::getGrabObject() const
{
    if (!m_grabObjectId.has_value()) {
        return nullptr;
    }
    return getObject(m_grabObjectId.value());
}

void InteractiveObjectView::setMovingCallback(const std::function<void (const QPointF &)> &callbackFunc)
{
    m_movingCallback = callbackFunc;
}

void InteractiveObjectView::setGrabObject(QGraphicsItem *pItem)
{
    if (nullptr == pItem) {
        throw std::invalid_argument("Can not set grab object to nullptr");
        return;
    }
    m_grabObjectId = pItem->data(ObjectViewConstants::ObjectField::OBJECTFIELD_ID).toLongLong();
    m_grabObjectPos = pItem->pos();
}

void InteractiveObjectView::acceptGrabObject()
{
    m_grabObjectId = std::nullopt;
}

void InteractiveObjectView::rejectGrabObject()
{
    if (!m_grabObjectId.has_value()) {
        throw std::runtime_error("InteractiveObjectView Error: can not reject without item grabbed");
    }
    auto grabObject = getGrabObject();
    grabObject->setPos(m_grabObjectPos);
    m_grabObjectId = std::nullopt;
}

void InteractiveObjectView::wheelEvent(QWheelEvent *e)
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

void InteractiveObjectView::mousePressEvent(QMouseEvent *e)
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

void InteractiveObjectView::mouseMoveEvent(QMouseEvent *e)
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

void InteractiveObjectView::mouseReleaseEvent(QMouseEvent *e)
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

void InteractiveObjectView::contextMenuEvent(QContextMenuEvent *e)
{
    m_mainContextMenu->exec(e->globalPos());
    QGraphicsView::contextMenuEvent(e);
}
