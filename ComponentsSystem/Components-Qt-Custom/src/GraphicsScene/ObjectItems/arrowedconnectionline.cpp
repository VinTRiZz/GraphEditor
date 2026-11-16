#include "arrowedconnectionline.hpp"

#include <Components/Logger/Logger.h>

namespace ObjectItems {

ArrowedConnectionLine::ArrowedConnectionLine(QGraphicsItem* parent) :
    AbstractConnectionLine(parent)
{
    setSystemName("Arrowed connection");
    setObjectType(ObjectType::OIT_ArrowedConnectionLine);

    createSubitem(m_line);
    m_line->setZValue(1);

    createSubitem(m_forwardArrow);
    m_forwardArrow->setZValue(2);
    m_forwardArrow->setBrush(getBackgroundBrush());
    m_forwardArrow->setPen(getLinePen());

    createSubitem(m_backwardArrow);
    m_backwardArrow->setZValue(2);
    m_backwardArrow->setBrush(getBackgroundBrush());
    m_backwardArrow->setPen(getLinePen());

    connect(this, &BasicItem::itemSelected,
            this, &ArrowedConnectionLine::updateLines);

    connect(this, &BasicItem::itemDeselected,
            this, &ArrowedConnectionLine::updateLines);

    connect(this, &BasicItem::graphicalDataChanged,
            this, &ArrowedConnectionLine::updateLines);
}

void ArrowedConnectionLine::updateLines()
{
    auto currentPen = getCurrentPen();
    auto currentBrush = getCurrentBrush();

    m_line->setLine(getLine());
    m_line->setPen(currentPen);

    m_forwardArrow->setBrush(currentBrush);
    m_forwardArrow->setPen(currentPen);
    m_backwardArrow->setBrush(currentBrush);
    m_backwardArrow->setPen(currentPen);

    auto arrowPath = createArrowPath();

    auto lineDirection = getDirection();
    if (lineDirection & LineDirectionType::Forward) {
        // Arrow
        m_forwardArrow->setPath(arrowPath);
        if (getIsArrowFilled()) {
            m_forwardArrow->setBrush(m_forwardArrow->pen().color());
        } else {
            m_forwardArrow->setBrush(Qt::transparent);
        }

        m_forwardArrow->setTransformOriginPoint(m_forwardArrow->boundingRect().center());
        m_forwardArrow->setRotation(90 - getLine().angle());

        // Position
        m_forwardArrow->setPos(getPositionTo() - m_forwardArrow->boundingRect().center());

        // Drawing
        m_forwardArrow->show();
    } else {
        m_forwardArrow->hide();
    }

    if (lineDirection & LineDirectionType::Backward) {
        // Arrow
        m_backwardArrow->setPath(arrowPath);
        if (getIsArrowFilled()) {
            m_backwardArrow->setBrush(m_backwardArrow->pen().color());
        } else {
            m_backwardArrow->setBrush(Qt::transparent);
        }

        m_backwardArrow->setTransformOriginPoint(m_backwardArrow->boundingRect().center());
        m_backwardArrow->setRotation(270 - getLine().angle());

        // Position
        m_backwardArrow->setPos(getPositionFrom() - m_backwardArrow->boundingRect().center());

        // Drawing
        m_backwardArrow->show();
    } else {
        m_backwardArrow->hide();
    }
}

void ArrowedConnectionLine::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    AbstractConnectionLine::hoverEnterEvent(e);
    updateLines();
}

void ArrowedConnectionLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    AbstractConnectionLine::hoverLeaveEvent(e);
    updateLines();
}

} // namespace ObjectItems

