#include "scenemarkeritem.h"

#include <QPainter>

#include <QGraphicsScene>
#include <QGraphicsView>

#include <math.h>

#include "constants.h"

namespace ObjectViewItems
{

SceneMarkerItem::SceneMarkerItem(ItemBase* parent) :
    ItemBase(parent)
{
    setSystemId();
    setSystemName("Маркер");
    setType(ObjectViewConstants::OBJECTTYPE_MARKER);

    setFlag(ItemIgnoresTransformations, true);

    // Дефолтная "картинка"
    m_targetImage = QImage(m_markerSize, QImage::Format_RGBA64);
    m_targetImage.fill(Qt::red);

    const qreal radius = m_markerSize.width() * 0.85;
    const qreal height = m_markerSize.height() * 2;

    QRectF circleRect(-radius, -radius, 2 * radius, 2 * radius);
    m_objectFigurePath.moveTo(-radius, 0);
    m_objectFigurePath.arcTo(circleRect, 180, -180);

    QPointF rightPoint(radius, 0);
    QPointF leftPoint(-radius, 0);
    QPointF bottomPoint(0, height);

    QPointF rightControl1(radius, height * 0.4);
    QPointF rightControl2(radius * 0.4, height);
    m_objectFigurePath.cubicTo(rightControl1, rightControl2, bottomPoint);

    QPointF leftControl1(-radius * 0.4, height);
    QPointF leftControl2(-radius, height * 0.4);
    m_objectFigurePath.cubicTo(leftControl1, leftControl2, leftPoint);

    QTransform transform;
    transform.rotate(-90);
    m_objectFigurePath = transform.map(m_objectFigurePath);
}

void SceneMarkerItem::setTarget(QGraphicsItem *pTarget)
{
    m_pTarget = pTarget;
}

QGraphicsItem *SceneMarkerItem::getTarget() const
{
    return m_pTarget;
}

void SceneMarkerItem::setTargetIcon(const QImage &targetImage)
{
    m_targetImage = targetImage;
}

void SceneMarkerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_pTarget == nullptr) {
        ItemBase::paint(painter, option, widget);
        return;
    }

    auto pSceneView = scene()->views().front();
    auto viewCenter = pSceneView->viewport()->rect().center();
    auto mappedCenter = pSceneView->mapToScene(viewCenter);

    auto targetLine = QLineF(mappedCenter, m_pTarget->pos());
    setRotation(-targetLine.angle());

    painter->setPen(QPen(Qt::black, 2));
    painter->drawPath(m_objectFigurePath);
    painter->fillPath(m_objectFigurePath, getMainColor());
    if (!m_targetImage.isNull()) {
        QRectF targetImageRect = m_objectFigurePath.boundingRect();
        targetImageRect.moveTo(-m_markerSize.width() / 2.0, -m_markerSize.height() / 2.0);
        targetImageRect.setWidth(m_markerSize.width());
        targetImageRect.setHeight(m_markerSize.height());
        painter->rotate(targetLine.angle());
        painter->drawImage(targetImageRect, m_targetImage);
    }
}

QVariant SceneMarkerItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemVisibleChange) {
        if (value.toBool()) {
            update();
        }
    }
    return ItemBase::itemChange(change, value);
}

}
