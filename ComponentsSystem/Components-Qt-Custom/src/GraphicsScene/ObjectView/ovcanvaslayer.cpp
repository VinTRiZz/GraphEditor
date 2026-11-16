#include "ovcanvaslayer.hpp"

#include "ovconstants.hpp"
#include "ovinternalscene.hpp"

#include <QWheelEvent>

#include <QScrollBar>

#include <QVariantAnimation>

#include <QGraphicsRectItem>

#include <Components/Logger/Logger.h>

namespace OVLayers {

OVCanvasLayer::OVCanvasLayer(QWidget *parent) :
    QGraphicsView(parent)
{
    m_pInternalScene = new OVInternalScene(this);
    setScene(m_pInternalScene);

    setRenderHint(QPainter::Antialiasing);  // красивые текстуры
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // Фикс артефактов Foreground
    setRubberBandSelectionMode(Qt::ItemSelectionMode::ContainsItemShape);   // Область выбора
    setDragMode(QGraphicsView::DragMode::RubberBandDrag);                   // Область выбора

    // Скрываем скроллбары
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    // Отключаем скролл обычной вьюшки чтобы зумить
    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);

    m_pCanvasItem = new QGraphicsRectItem;
    m_pInternalScene->addItem(m_pCanvasItem);
    m_pCanvasItem->setBrush(DEFAULT_CANVASCOLOR);
    m_pCanvasItem->setPen(QPen(DEFALT_CANVASBORDERCOLOR, 2));
    m_pCanvasItem->setZValue(ItemLayers::CanvasLayer);
    m_pCanvasItem->show();
}

void OVCanvasLayer::setCanvasRect(const QRectF &iRect) {
    m_pCanvasItem->setRect(iRect);
    resetTransform();

    QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();
    auto viewCenter = viewRect.center();

    auto deltaPos = iRect.center() - viewCenter;
    horizontalScrollBar()->setSliderPosition(
                horizontalScrollBar()->sliderPosition() + deltaPos.x());
    verticalScrollBar()->setSliderPosition(
                verticalScrollBar()->sliderPosition() + deltaPos.y());

    auto rectCopy = iRect;
    rectCopy.moveTo(-10, -10);
    rectCopy.setSize(QSize(rectCopy.width() + 10, rectCopy.height() + 10));
    setSceneRect(rectCopy);
}

QRectF OVCanvasLayer::getCanvasRect() const
{
    return m_pCanvasItem->rect();
}

OVInternalScene *OVCanvasLayer::getScene() const
{
    return m_pInternalScene;
}

QGraphicsRectItem *OVCanvasLayer::getCanvas() const
{
    return m_pCanvasItem;
}

ObjectItems::BasicItem *OVCanvasLayer::getObject(const QPoint &viewportPos) const
{
    auto topItem = getTopItem(viewportPos);
    if (nullptr == topItem) {
        return nullptr;
    }
    auto parentObjectItem = topItem->data(ObjectItems::ObjectDataRole::OBJECTDATAROLE_PARENTITEM_POINTER);
    if (nullptr == parentObjectItem.value<ObjectItems::BasicItem*>()) {
        return dynamic_cast<ObjectItems::BasicItem*>(topItem);
    }
    return qvariant_cast<ObjectItems::BasicItem*>(parentObjectItem);
}

QGraphicsItem *OVCanvasLayer::getTopItem(const QPoint &viewportPos) const
{
    auto posItems = getItems(viewportPos, true);
    return (posItems.empty() ? nullptr : posItems.front());
}

QList<QGraphicsItem *> OVCanvasLayer::getItems(const QPoint &viewportPos, bool sorted) const
{
    auto posItems = items(viewportPos);
    if (sorted) {
        std::sort(posItems.begin(), posItems.end(), [](auto* pItemL, auto* pItemR){
            return (pItemL->zValue() > pItemR->zValue());
        });
    }
    auto removedBeg = std::remove_if(posItems.begin(), posItems.end(), [this](auto* pItem){
        return isSystemItem(pItem);
    });
    posItems.erase(removedBeg, posItems.end());
    return posItems;
}

bool OVCanvasLayer::isSystemItem(QGraphicsItem *pItem) const
{
    return  (pItem->zValue() <= ItemLayers::CanvasLayer) &&
            (pItem->zValue() >= ItemLayers::SystemComponentsLayerBegin);
}

void OVCanvasLayer::zoomIn() {
    auto anim = new QVariantAnimation(this);
    anim->setStartValue(0.99);
    anim->setEndValue(0.9);
    anim->setDuration(100);
    connect(anim, &QVariantAnimation::valueChanged,
            this, [this](const QVariant& deltaV){
        customZoom(deltaV.toDouble());
    });
    anim->start(QVariantAnimation::DeleteWhenStopped);
}

void OVCanvasLayer::zoomOut() {
    auto anim = new QVariantAnimation(this);
    anim->setStartValue(1.01);
    anim->setEndValue(1.1);
    anim->setDuration(100);
    connect(anim, &QVariantAnimation::valueChanged,
            this, [this](const QVariant& deltaV){
        customZoom(deltaV.toDouble());
    });
    anim->start(QVariantAnimation::DeleteWhenStopped);
}

void OVCanvasLayer::customZoom(double scaleCoeff) {
    scale(scaleCoeff, scaleCoeff);
    setSceneRect(getCanvas()->rect()); // Чтобы не было багов
    emit scaleChanged(getCurrentScale());
}

double OVCanvasLayer::getCurrentScale() const {
    return transform().m11();
}

bool OVCanvasLayer::isIdFree(ObjectItems::objectId_t id) const
{
    return (m_registeredItems.count(id) == 0);
}

ObjectItems::objectId_t OVCanvasLayer::getFreeSystemId() const
{
    ObjectItems::objectId_t res {-1};
    while (m_registeredItems.count(res) != 0) {
        --res;

        if (res == std::numeric_limits<ObjectItems::objectId_t>::min()) {
            throw std::runtime_error("OVCanvasLayer: No free system id");
        }
    }
    return res;
}

ObjectItems::objectId_t OVCanvasLayer::getFreeObjectId() const
{
    ObjectItems::objectId_t res {1};
    while (m_registeredItems.count(res) != 0) {
        ++res;
        if (res == std::numeric_limits<ObjectItems::objectId_t>::max()) {
            throw std::runtime_error("OVCanvasLayer: No free object id");
        }
    }
    return res;
}

void OVCanvasLayer::addObject(ObjectItems::BasicItem *pItem)
{
    if (0 == pItem->getItemId()) {
        COMPLOG_ERROR("OVCanvasLayer: Can not add item with id 0");
        return;
    }
    getScene()->addItem(pItem);
    if (m_registeredItems.count(pItem->getItemId()) != 0) {
        COMPLOG_WARNING("OVCanvasLayer: Rewriting object with id:", pItem->getItemId());
    }
    m_registeredItems[pItem->getItemId()] = pItem;
    connect(pItem, &QObject::destroyed,
            this, [this, pItem](){ m_registeredItems.erase(pItem->getItemId()); });
}

std::unordered_map<ObjectItems::objectId_t, ObjectItems::BasicItem *> OVCanvasLayer::getObjects() const
{
    return m_registeredItems;
}

ObjectItems::BasicItem *OVCanvasLayer::getObject(ObjectItems::objectId_t itemId) const
{
    auto targetObject = m_registeredItems.find(itemId);
    if (targetObject != m_registeredItems.end()) {
        return targetObject->second;
    }
    return nullptr;
}

void OVCanvasLayer::removeObject(ObjectItems::BasicItem *pItem)
{
    m_registeredItems.erase(pItem->getItemId());
    getScene()->removeItem(pItem);
    delete pItem;
}

void OVCanvasLayer::setNavigationEnabled(bool isEn)
{
    m_isNavigationEnabled = isEn;
}

void OVCanvasLayer::resetScale()
{
    resetTransform();
}


void OVCanvasLayer::wheelEvent(QWheelEvent* e) {
    QGraphicsView::wheelEvent(e);

    if (!m_isNavigationEnabled) {
        return;
    }

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

void OVCanvasLayer::mousePressEvent(QMouseEvent* e) {
    QGraphicsView::mousePressEvent(e);

    if (!m_isNavigationEnabled) {
        return;
    }

    m_isHoldingMiddleButton = (e->buttons() & Qt::MiddleButton);
    if (m_isHoldingMiddleButton) {
        setCursor(Qt::SizeAllCursor);
        m_prevPos = mapToScene(e->pos());
    }
}

void OVCanvasLayer::mouseMoveEvent(QMouseEvent* e) {
    QGraphicsView::mouseMoveEvent(e);

    if (!m_isNavigationEnabled) {
        return;
    }

    if (m_isHoldingMiddleButton) {
        auto deltaPos = e->pos() - mapFromScene(m_prevPos);
        horizontalScrollBar()->setSliderPosition(
            horizontalScrollBar()->sliderPosition() - deltaPos.x());
        verticalScrollBar()->setSliderPosition(
            verticalScrollBar()->sliderPosition() - deltaPos.y());
        emit visibleRectMoved();
    }
}

void OVCanvasLayer::mouseReleaseEvent(QMouseEvent* e) {
    QGraphicsView::mouseReleaseEvent(e);

    if (!m_isNavigationEnabled) {
        return;
    }

    setCursor(Qt::ArrowCursor);
    m_isHoldingMiddleButton = false;
}

bool OVCanvasLayer::eventFilter(QObject *object, QEvent *event)
{
    // Отключение скролла обычной QGraphicsView
    if ((object == horizontalScrollBar() || object == verticalScrollBar()) && event->type() == QEvent::Wheel) {
        return true;
    }
    return QGraphicsView::eventFilter(object, event);
}


}
