#include "markeritem.hpp"

#include <Components/CustomQt/ObjectView/CanvasLayer.h>

#include <QVariantAnimation>

#include <math.h>

#include <Components/Logger/Logger.h>

namespace ObjectItems {

MarkerItem::MarkerItem(QGraphicsItem* parent)
    : BasicItem(parent)
    , m_view(nullptr)
{
    setSystemName("Marker");
    setObjectType(ObjectType::OIT_MarkerItem);

    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsFocusable, false);
    setFlag(ItemIgnoresTransformations, true);

    createSubitem(m_markerVisibleItem);
    m_markerVisibleItem->setPath(createMarkerPath());
    m_markerVisibleItem->setTransformOriginPoint(m_markerVisibleItem->boundingRect().center());
    setTransformOriginPoint(boundingRect().center());

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_markerVisibleItem->setPen(getLinePen());
        m_markerVisibleItem->setBrush(getBackgroundBrush());
    });

    setLinePen({Qt::black});
    setBackgroundBrush(Qt::lightGray);
}

void MarkerItem::setTrackedItem(BasicItem* item)
{
    if (m_trackedItem) {
        disconnect(m_trackedItem, &BasicItem::itemMovedOnScene,
                this, &MarkerItem::updateMarkerPosition);
    }
    m_trackedItem = item;
    connect(item, &BasicItem::itemMovedOnScene,
            this, &MarkerItem::updateMarkerPosition);
    updateMarkerPosition();
}

void MarkerItem::setView(OVLayers::OVCanvasLayer *view)
{
    if (m_view) {
        disconnect(this, nullptr, m_view, nullptr);
    }
    m_view = view;
    connect(this, &BasicItem::itemClicked,
            m_view, [this](){
        m_view->centerOn(calculateTargetCenter());
        updateMarkerPosition();

        auto pAnim = new QVariantAnimation(this);
        pAnim->setStartValue(std::sqrt(150));
        pAnim->setEndValue(std::sqrt(250));
        pAnim->setDuration(500);

        connect(pAnim, &QVariantAnimation::valueChanged,
                this, [this](const QVariant& deltaValue){
            m_markerVisibleItem->setBrush(QColor(120, std::pow(deltaValue.toInt(), 2), 120));
        });

        connect(pAnim, &QVariantAnimation::finished,
               this, [this](){
            m_markerVisibleItem->setBrush(getBackgroundBrush());
        });

        pAnim->start(QVariantAnimation::DeleteWhenStopped);
    });
    connect(view, &OVLayers::OVCanvasLayer::scaleChanged,
            this, &MarkerItem::updateMarkerPosition);
    connect(view, &OVLayers::OVCanvasLayer::visibleRectMoved,
            this, &MarkerItem::updateMarkerPosition);
    updateMarkerPosition();
}

QPointF MarkerItem::calculateTargetCenter() const
{
    if (m_trackedItem == nullptr) {
        return QPointF();
    }
    QRectF itemRect = m_trackedItem->mapToScene(m_trackedItem->boundingRect()).boundingRect();
    return itemRect.center();
}

QPointF MarkerItem::calculateMarkerPosition(const QPointF& targetCenter) const
{
    if (!m_view || !scene()) {
        return QPointF(targetCenter.x(), targetCenter.y() - m_markerVisibleItem->boundingRect().bottom() / m_view->getCurrentScale());
    }

    QRect viewportRect = m_view->viewport()->rect();
    QRectF sceneRect = m_view->mapToScene(viewportRect).boundingRect();
    if (sceneRect.contains(targetCenter)) {
        return QPointF(targetCenter.x(), targetCenter.y() - m_markerVisibleItem->boundingRect().bottom() / m_view->getCurrentScale());
    }

    QPointF viewportCenter = sceneRect.center();
    QLineF directionLine(viewportCenter, targetCenter);

    const double markerOffset = m_markerVisibleItem->boundingRect().bottom() / m_view->getCurrentScale();
    sceneRect.setWidth(sceneRect.width() - markerOffset * 2.0);
    sceneRect.setHeight(sceneRect.height() - markerOffset * 2.0);
    sceneRect.moveTo(sceneRect.x() + markerOffset, sceneRect.y() + markerOffset);

    std::array<QLineF, 4> boundaryLines = {
        QLineF(sceneRect.topLeft(), sceneRect.topRight()),
        QLineF(sceneRect.topRight(), sceneRect.bottomRight()),
        QLineF(sceneRect.bottomRight(), sceneRect.bottomLeft()),
        QLineF(sceneRect.bottomLeft(), sceneRect.topLeft())
    };

    QPointF intersectionPoint;
    for (const QLineF& boundary : boundaryLines) {
        QPointF intersect;
        if (directionLine.intersects(boundary, &intersect) == QLineF::BoundedIntersection) {
            intersectionPoint = intersect;
            break;
        }
    }

    return intersectionPoint;
}

void MarkerItem::updateMarkerPosition()
{
    if (!m_trackedItem || !m_view) {
        setVisible(false);
        return;
    }

    QPointF targetCenter = calculateTargetCenter();
    QPointF markerPos = calculateMarkerPosition(targetCenter);
    double rotationAngle {0};
    if (targetCenter != markerPos) {
        rotationAngle = 270 - QLineF(markerPos, targetCenter).angle();
    }

    setPos(markerPos);
    m_markerVisibleItem->setRotation(rotationAngle);
    setVisible(true);
}

void MarkerItem::setMarkerHeight(double marHeight)
{
    m_markerHeight = marHeight;
    m_isHeightChanged = true;
    m_markerVisibleItem->setPath(createMarkerPath());
    m_markerVisibleItem->setTransformOriginPoint(m_markerVisibleItem->boundingRect().center());
    updateMarkerPosition();
}

QPainterPath MarkerItem::createMarkerPath() const
{
    if (!m_isHeightChanged) {
        return m_cachedMarkerPath;
    }

    QPainterPath path;

    auto sideSize = m_markerHeight * std::sin(M_PI / 180.0 * 15);
    auto backsideSize = m_markerHeight * 0.2;

    path.lineTo(QPoint(-sideSize, -backsideSize));
    path.lineTo(QPoint(0, m_markerHeight));
    path.lineTo(QPoint(sideSize, -backsideSize));
    path.lineTo(QPoint(0, 0));

    m_cachedMarkerPath = path;
    m_isHeightChanged = false;
    return path;
}

void MarkerItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    m_markerVisibleItem->setPen(getLineHoverPen());
    m_markerVisibleItem->setBrush(getBackgroundHoverBrush());
    BasicItem::hoverEnterEvent(e);
}

void MarkerItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    m_markerVisibleItem->setPen(getLinePen());
    m_markerVisibleItem->setBrush(getBackgroundBrush());
    BasicItem::hoverLeaveEvent(e);
}

} // namespace ObjectItems
