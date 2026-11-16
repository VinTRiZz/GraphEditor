#include "elegantconnectionline.hpp"

#include <math.h>

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ElegantConnectionLine::ElegantConnectionLine(QGraphicsItem *parent) :
    AbstractConnectionLine(parent)
{
    setSystemName("Elegant connection");
    setObjectType(ObjectType::OIT_ElegantConnectionLine);

    createSubitem(m_line);
    m_line->setZValue(1);
    createSubitem(m_lineSelected);
    m_lineSelected->hide();

    createSubitem(m_forwardArrow);
    m_forwardArrow->setZValue(2);
    m_forwardArrow->setBrush(getBackgroundBrush());
    m_forwardArrow->setPen(getLinePen());
    m_forwardArrow->setRotation(180);

    createSubitem(m_backwardArrow);
    m_backwardArrow->setZValue(2);
    m_backwardArrow->setBrush(getBackgroundBrush());
    m_backwardArrow->setPen(getLinePen());

    connect(this, &BasicItem::itemSelected,
            this, [this](){
        m_lineSelected->show();
        m_forwardArrow->setPen(m_lineSelected->pen());
        m_backwardArrow->setPen(m_lineSelected->pen());
    });

    connect(this, &BasicItem::itemDeselected,
            this, [this](){
        m_lineSelected->hide();
        m_forwardArrow->setPen(getLinePen());
        m_backwardArrow->setPen(getLinePen());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        auto linePen = getLinePen();

        m_line->setPath(createLinePath());
        m_line->setPen(linePen);
        m_line->setBrush(getBackgroundBrush());

        m_lineSelected->setPath(m_line->path());
        linePen = getLineSelectionPen();
        m_lineSelected->setPen(linePen);

        auto lineDirection = getDirection();
        if (lineDirection & LineDirectionType::Forward) {
            // Arrow
            m_forwardArrow->setPath(createArrowPath());
            if (getIsArrowFilled()) {
                m_forwardArrow->setBrush(m_forwardArrow->pen().color());
            } else {
                m_forwardArrow->setBrush(Qt::transparent);
            }
            m_forwardArrow->setTransformOriginPoint(m_forwardArrow->boundingRect().center());

            // Position
            m_forwardArrow->setPos(getPositionTo() - m_forwardArrow->boundingRect().center());

            // Drawing
            m_forwardArrow->show();
        } else {
            m_forwardArrow->hide();
        }

        if (lineDirection & LineDirectionType::Backward) {
            // Arrow
            m_backwardArrow->setPath(createArrowPath());
            if (getIsArrowFilled()) {
                m_backwardArrow->setBrush(m_backwardArrow->pen().color());
            } else {
                m_backwardArrow->setBrush(Qt::transparent);
            }
            m_backwardArrow->setTransformOriginPoint(m_backwardArrow->boundingRect().center());

            // Position
            m_backwardArrow->setPos(getPositionFrom() - m_backwardArrow->boundingRect().center());

            // Drawing
            m_backwardArrow->show();
        } else {
            m_backwardArrow->hide();
        }
    });
}

QPainterPath ElegantConnectionLine::createLinePath() const
{
    auto arrowHeight = getArrowHeight();

    auto pointFrom = getPositionFrom();
    pointFrom.setY(pointFrom.y() + arrowHeight);

    auto pointTo = getPositionTo();
    pointTo.setY(pointTo.y() - arrowHeight);

    auto centerPoint = getLine().center();
    auto referPoint1 = centerPoint;
    referPoint1.setY(pointFrom.y());

    auto referPoint2 = centerPoint;
    referPoint2.setY(pointTo.y());

    QPainterPath p;
    p.moveTo(getPositionFrom());
    p.lineTo(pointFrom);
    p.cubicTo(pointFrom, referPoint1, centerPoint);
    p.cubicTo(centerPoint, referPoint2, pointTo);
    p.lineTo(pointTo);
    p.lineTo(getPositionTo());

    // Для правильной области определения
    QPainterPathStroker stroker;
    stroker.setWidth(getLinePen().widthF());
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);

    return stroker.createStroke(p);
}

} // namespace ObjectItems
