#include "vertexconnectionline.h"

#include <QStyleOptionGraphicsItem>

#include <QGraphicsSceneMouseEvent>

#include <QPainter>
#include <QPainterPath>

#include <math.h>

#include "logging.h"

namespace PredefinedObjects
{

VertexConnectionLine::VertexConnectionLine(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    m_drawPen {QPen(Qt::black, 2)},
    m_selectedPen {QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap)}
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    for (int i = 0; i < 6; ++i) {
        auto pItem = new QGraphicsLineItem(this);
        m_lines.push_back(pItem);
    }
    m_pArrowHeadPolygon = new QGraphicsPolygonItem(this);
}

void VertexConnectionLine::setLine(const QLineF &line)
{
    m_straightLine = line;
    updatePolygon();
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
    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    for (auto& pItem : m_lines) {
        pItem->setPen(currentPen);
    }
    m_pArrowHeadPolygon->setPen(currentPen);
}

void VertexConnectionLine::setSelectedPen(const QPen &pen)
{
    m_selectedPen = pen;
    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    for (auto& pItem : m_lines) {
        pItem->setPen(currentPen);
    }
    m_pArrowHeadPolygon->setPen(currentPen);
}

void VertexConnectionLine::setArrowSize(qreal size)
{
    m_arrowSize = size;
}

qreal VertexConnectionLine::getArrowSize() const
{
    return m_arrowSize;
}

void VertexConnectionLine::paint([[maybe_unused]] QPainter *painter, [[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{

}

void VertexConnectionLine::updatePolygon()
{
    m_boundingRect = {};

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

    auto arrowLine = QLineF(pointTo, m_straightLine.p2());
    if (arrowLine.length() != 0) {
        // Угол линии
        double angle = (arrowLine.angle() + 180) * M_PI / 180.0;

        const auto PI_DELIM_3 = M_PI / 3;
        const auto PI_2_DELIM_3 = M_PI * 2 / 3;

        QPointF arrowP1 = arrowLine.p2();

        QPointF arrowP2 = arrowLine.p2() + QPointF(sin(angle + PI_DELIM_3) * m_arrowSize,
                                              cos(angle + PI_DELIM_3) * m_arrowSize);

        QPointF arrowP3 = arrowLine.p2() + QPointF(sin(angle + PI_2_DELIM_3) * m_arrowSize,
                                              cos(angle + PI_2_DELIM_3) * m_arrowSize);

        QPolygonF arrowHeadPolygon = {arrowP1, arrowP2, arrowP3};
        m_pArrowHeadPolygon->setPolygon(arrowHeadPolygon);
        (*std::prev(m_lines.end()))->setLine(arrowLine);

        m_pArrowHeadPolygon->show();
        (*std::prev(m_lines.end()))->show();
    } else {
        m_pArrowHeadPolygon->hide();
        (*std::prev(m_lines.end()))->hide();
    }

    auto currentLineIt = m_lines.begin();
    (*currentLineIt)->setLine(QLineF(pointFrom, firstBreak));

    if (pointFrom.y() > pointTo.y()) {
        QPointF correctionBreak1;
        correctionBreak1.setX(m_straightLine.x1() + m_straightLine.dx() / 2);
        correctionBreak1.setY(firstBreak.y());

        QPointF correctionBreak2;
        correctionBreak2.setX(correctionBreak1.x());
        correctionBreak2.setY(m_straightLine.y2() - m_arrowSize * 2);

        currentLineIt = std::next(currentLineIt);
        (*currentLineIt)->setLine(QLineF(firstBreak, correctionBreak1));

        currentLineIt = std::next(currentLineIt);
        (*currentLineIt)->setLine(QLineF(correctionBreak1, correctionBreak2));
        (*currentLineIt)->show();

        currentLineIt = std::next(currentLineIt);
        (*currentLineIt)->setLine(QLineF(correctionBreak2, secondBreak));
        (*currentLineIt)->show();

        currentLineIt = std::next(currentLineIt);
        (*currentLineIt)->setLine(QLineF(secondBreak, pointTo));

        QPointF boundingTargetPos = QPointF(m_straightLine.x1() > m_straightLine.x2() ? m_straightLine.x2(): m_straightLine.x1(),
                                            firstBreak.y() > secondBreak.y() ? secondBreak.y() : firstBreak.y());
        boundingTargetPos.setX(boundingTargetPos.x() - m_arrowSize);
        m_boundingRect.moveTo(boundingTargetPos);
        m_boundingRect.setWidth(std::fabs(firstBreak.x() - secondBreak.x()) + m_arrowSize * 2);
        m_boundingRect.setHeight(std::fabs(firstBreak.y() - secondBreak.y()));
        return;
    }

    currentLineIt = std::next(currentLineIt);
    (*currentLineIt)->setLine(QLineF(firstBreak, secondBreak));

    currentLineIt = std::next(currentLineIt);
    (*currentLineIt)->hide();

    currentLineIt = std::next(currentLineIt);
    (*currentLineIt)->hide();

    currentLineIt = std::next(currentLineIt);
    (*currentLineIt)->setLine(QLineF(secondBreak, pointTo));

    auto movePos = m_straightLine.p1();
    movePos.setX(movePos.x() - m_arrowSize);
    m_boundingRect.moveTo(movePos);
    m_boundingRect.setWidth(std::fabs(m_straightLine.p2().x() - m_straightLine.x1()) + m_arrowSize * 2);
    m_boundingRect.setHeight(std::fabs(m_straightLine.p2().y() - m_straightLine.y1()));
}

void VertexConnectionLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void VertexConnectionLine::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    auto isContain = contains(mapToScene(event->pos()));
    setSelected(isContain ^ isSelected());
    updatePen();
}

void VertexConnectionLine::updatePen()
{
    if (isSelected() == m_prevSelectedState) {
        return;
    }
    m_prevSelectedState = isSelected();

    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    for (auto& pItem : m_lines) {
        pItem->setPen(currentPen);
    }
    m_pArrowHeadPolygon->setPen(currentPen);
}

QRectF VertexConnectionLine::boundingRect() const
{
    return m_boundingRect;
}

bool VertexConnectionLine::contains(const QPointF &p) const
{
    for (auto& line : m_lines) {
        if (line->contains(p)) {
            return true;
        }
    }
    return m_pArrowHeadPolygon->contains(p);
}

QPainterPath VertexConnectionLine::shape() const
{
    QPainterPath res;

    for (auto& line : m_lines) {
        res.addPath(line->shape());
    }
    res.addPath(m_pArrowHeadPolygon->shape());
    return res;
}

}
