#include "vertexconnectionline.h"

#include <QStyleOptionGraphicsItem>

#include <QPainter>

#include <math.h>

#include "logging.h"


namespace PredefinedObjects
{

VertexConnectionLine::VertexConnectionLine(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_drawPen {QPen(Qt::black, 1)},
    m_selectedPen {QPen(Qt::yellow, 3)}
{

}

void VertexConnectionLine::setPositionFrom(const QPointF &posFrom)
{
    m_straightLine.setP1(posFrom);
    updatePolygon();
}

void VertexConnectionLine::setPositionTo(const QPointF &posTo)
{
    m_straightLine.setP2(posTo);
    updatePolygon();
}

void VertexConnectionLine::setPen(const QPen &pen)
{
    m_drawPen = pen;
}

void VertexConnectionLine::setSelectedPen(const QPen &pen)
{
    m_selectedPen = pen;
}

void VertexConnectionLine::setArrowSize(qreal size)
{
    m_arrowSize = size;
}

qreal VertexConnectionLine::arrowSize() const
{
    return m_arrowSize;
}

void VertexConnectionLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (option->state & QStyle::StateFlag::State_Selected) {
        painter->setPen(m_selectedPen);
    } else {
        painter->setPen(m_drawPen);
    }

    painter->drawLine(m_firstLine);

    // Всегда будет true если Y2 < Y1
    if (!m_correctionVerticalLine.isNull() || !m_correctionHorizontalLine.isNull()) {
        painter->drawLine(m_correctionVerticalLine);
        painter->drawLine(m_correctionHorizontalLine);
    }

    painter->drawLine(m_secondLine);
    painter->drawLine(m_thirdLine);
    drawArrow(painter);

    Q_UNUSED(widget);
}

void VertexConnectionLine::updatePolygon()
{
    auto lineGoRightSide = m_straightLine.dx() > 0;

    auto pointFrom = m_straightLine.p1();
    auto pointTo = m_straightLine.p2();

    auto linePart = (fabs(pointFrom.y() - pointTo.y()) * 1.2 ) / 3.0;

    QPointF firstBreak;
    firstBreak.setX(pointFrom.x());
    firstBreak.setY(pointFrom.y() + linePart);

    QPointF secondBreak;
    secondBreak.setX(pointTo.x() - (!lineGoRightSide * -1.0 + lineGoRightSide) * m_arrowSize);
    secondBreak.setY(pointTo.y() - linePart);

    pointTo.setX(pointTo.x() - (-1 * !lineGoRightSide + lineGoRightSide) * m_arrowSize);
    pointTo.setY(pointTo.y() - m_arrowSize);

    if (pointFrom.y() > pointTo.y()) {
        QPointF correctionBreak1;
        correctionBreak1.setX(m_straightLine.x1() + m_straightLine.dx() / 2);
        correctionBreak1.setY(firstBreak.y());
        m_correctionHorizontalLine = QLineF(firstBreak, correctionBreak1);

        QPointF correctionBreak2;
        correctionBreak2.setX(correctionBreak1.x());
        correctionBreak2.setY(m_straightLine.y2() - m_arrowSize * 2);
        m_correctionVerticalLine = QLineF(correctionBreak1, correctionBreak2);

        m_firstLine = QLineF(pointFrom, firstBreak);
        m_secondLine = QLineF(correctionBreak2, secondBreak);
        m_thirdLine = QLineF(secondBreak, pointTo);
        return;
    }

    m_correctionHorizontalLine = {};
    m_correctionVerticalLine = {};

    m_firstLine = QLineF(pointFrom, firstBreak);
    m_secondLine = QLineF(firstBreak, secondBreak);
    m_thirdLine = QLineF(secondBreak, pointTo);
}

void VertexConnectionLine::drawArrow(QPainter *painter)
{
    QLineF line(m_thirdLine.p2(), m_straightLine.p2());
    if (line.length() == 0)
        return;

    // Угол линии
    double angle = (line.angle() + 180) * M_PI / 180.0;

    static auto PI_DELIM_3 = M_PI / 3;
    static auto PI_2_DELIM_3 = M_PI * 2 / 3;

    QPointF arrowP1 = line.p2();

    QPointF arrowP2 = line.p2() + QPointF(sin(angle + PI_DELIM_3) * m_arrowSize,
                                          cos(angle + PI_DELIM_3) * m_arrowSize);

    QPointF arrowP3 = line.p2() + QPointF(sin(angle + PI_2_DELIM_3) * m_arrowSize,
                                          cos(angle + PI_2_DELIM_3) * m_arrowSize);

    QPolygonF arrowHead( {arrowP1, arrowP2, arrowP3} );

    painter->setBrush(painter->pen().color());
    painter->drawLine(line);
    painter->drawPolygon(arrowHead);
}

QRectF VertexConnectionLine::boundingRect() const
{
    return QRectF(m_straightLine.x1(), m_straightLine.y1(), fabs(m_straightLine.dx()), fabs(m_straightLine.dy()));
}

}
