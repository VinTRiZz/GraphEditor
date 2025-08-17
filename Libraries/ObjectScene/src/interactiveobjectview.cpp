#include "interactiveobjectview.h"

#include <Common/Logging.h>

#include <QMenu>
#include <QScrollBar>
#include <QWheelEvent>

InteractiveObjectView::InteractiveObjectView(QWidget* parent)
    : ObjectViewBase(parent) {
    m_mainContextMenu = new QMenu(this);

    m_mainContextMenu->addAction("Переключить сетку", [this]() {
        scene()->setGridEnabled(!scene()->getIsGridEnabled());
    });

    m_pCenterItem = new ObjectViewItems::CenterItem();
    addObject(m_pCenterItem);

    m_centerPointerItem = new ObjectViewItems::SceneMarkerItem();
    m_centerPointerItem->setTarget(m_pCenterItem);
    m_centerPointerItem->setMainColor(QColor(5, 5, 5, 15));

    // TODO: Рисовать иначе
    QImage centerImage (50, 50, QImage::Format_RGBA64);
    QPainter p(&centerImage);

    const double maxX = 50;
    const double maxY = 50;
    const double roundEllipseScale = 0.4;

    const double centerSizeX = 10;
    const double centerSizeY = 10;

    p.setPen(QPen(QColor(255, 0, 0, 170), 3));
    p.setBrush(QColor(255, 0, 0, 170));
    p.drawEllipse(QRectF(maxX / 2.0 - centerSizeX / 2.0, maxY / 2.0 - centerSizeY / 2.0, centerSizeX, centerSizeY));

    p.setPen(QPen(QColor(255, 0, 0, 170), 3));
    p.setBrush(Qt::transparent);
    p.drawEllipse(QRectF(maxX * (1 - roundEllipseScale) * 0.5, maxY * (1 - roundEllipseScale) * 0.5,
                         maxX * roundEllipseScale, maxY * roundEllipseScale));

    p.setPen(QPen(Qt::black, 3));
    p.drawLine(QLineF(maxX / 2.0, 0, maxX / 2.0, maxY));
    p.drawLine(QLineF(0, maxY / 2.0, maxX, maxY / 2.0));
    m_centerPointerItem->setTargetIcon(centerImage);

    addObject(m_centerPointerItem);
}

void InteractiveObjectView::zoomIn() {
    scale(1.2, 1.2);
    updateCenterPoint();
    emit scaleChanged();
}

void InteractiveObjectView::zoomOut() {
    scale(0.8, 0.8);
    updateCenterPoint();
    emit scaleChanged();
}

void InteractiveObjectView::customZoom(double scaleCoeff) {
    scale(scaleCoeff, scaleCoeff);
    updateCenterPoint();
    emit scaleChanged();
}

double InteractiveObjectView::getCurrentScale() const {
    return transform().m11();
}

void InteractiveObjectView::setCanvasRect(const QRectF &iRect)
{
    ObjectViewBase::setCanvasRect(iRect);
    m_pCenterItem->setPos(iRect.center() - m_pCenterItem->boundingRect().center());
}

void InteractiveObjectView::setContextMenu(QMenu* pMenu) {
    m_mainContextMenu->addMenu(pMenu);
}

QGraphicsItem* InteractiveObjectView::getContextMenuItem() {
    return m_contextMenuItem;
}

QGraphicsItem* InteractiveObjectView::getGrabObject() const {
    if (!m_grabObjectId.has_value()) {
        return nullptr;
    }
    return getObject(m_grabObjectId.value());
}

void InteractiveObjectView::setMovingCallback(
    const std::function<void(const QPointF&)>& callbackFunc) {
    m_movingCallback = callbackFunc;
}

void InteractiveObjectView::setGrabObject(QGraphicsItem* pItem) {
    if (nullptr == pItem) {
        throw std::invalid_argument("Can not set grab object to nullptr");
        return;
    }
    m_grabObjectId =
        pItem->data(ObjectViewConstants::ObjectField::OBJECTFIELD_ID)
            .toLongLong();
    m_grabObjectPos = pItem->pos();
}

void InteractiveObjectView::acceptGrabObject() {
    m_grabObjectId = std::nullopt;
}

void InteractiveObjectView::rejectGrabObject() {
    if (!m_grabObjectId.has_value()) {
        throw std::runtime_error(
            "InteractiveObjectView Error: can not reject without item grabbed");
    }
    auto grabObject = getGrabObject();
    grabObject->setPos(m_grabObjectPos);
    m_grabObjectId = std::nullopt;
}

void InteractiveObjectView::updateCenterPoint()
{
    auto targetPos = getCanvasRect().center() - (m_pCenterItem->boundingRect().center() / getCurrentScale());
    m_pCenterItem->setPos(targetPos);
    updateCenterMarker();
}

void InteractiveObjectView::updateCenterMarker()
{
    QRectF viewportRect = mapToScene(viewport()->geometry()).boundingRect();
    QPointF targetPos = m_pCenterItem->scenePos();

    if (viewportRect.contains(targetPos)) {
        m_centerPointerItem->hide();
    } else {
        m_centerPointerItem->show();

        auto pTarget = m_centerPointerItem->getTarget();
        auto targetLine = QLineF(viewportRect.center(), pTarget->pos());
        m_centerPointerItem->setPos(targetLine.pointAt(viewportRect.height() * 0.4 / targetLine.length()));
    }
}

void InteractiveObjectView::wheelEvent(QWheelEvent* e) {
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

void InteractiveObjectView::mousePressEvent(QMouseEvent* e) {
    setCursor(Qt::ArrowCursor);

    m_isHoldingLeftButton = (e->button() == Qt::LeftButton);
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

void InteractiveObjectView::mouseMoveEvent(QMouseEvent* e) {
    auto currentPos = mapToScene(e->pos());

    if (m_grabObjectId.has_value()) {
        auto pObject = getGrabObject();
        pObject->setPos(currentPos - pObject->boundingRect().center());
    }

    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(
            horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(
            verticalScrollBar()->sliderPosition() - deltaPos.y());
        updateCenterMarker();
    }

    if (m_movingCallback) {
        m_movingCallback(mapToScene(e->pos()));
    }
}

void InteractiveObjectView::mouseReleaseEvent(QMouseEvent* e) {
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

void InteractiveObjectView::contextMenuEvent(QContextMenuEvent* e) {
    m_mainContextMenu->exec(e->globalPos());
    QGraphicsView::contextMenuEvent(e);
}
