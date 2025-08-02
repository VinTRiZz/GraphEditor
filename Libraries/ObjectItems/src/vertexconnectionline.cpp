#include "vertexconnectionline.h"

#include <QStyleOptionGraphicsItem>

#include <QGraphicsSceneMouseEvent>

#include <QPainter>
#include <QPainterPath>

#include <math.h>

#include <Common/Logging.h>

#include <GraphObject/Object.h>

#include <QJsonObject>

#include "vertexobject.h"
#include "constants.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems
{

VertexConnectionLine::VertexConnectionLine(QGraphicsItem *parent) :
    ItemBase(parent)
{
    setSystemName("Соединение вершин");

    setType(ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION);

    m_penSelectedGradient.setColorAt(0, QColor("#fff09c"));
    m_penSelectedGradient.setColorAt(1, QColor("#ffbc20"));
    m_selectedPen.setWidth(8);
    m_selectedPen.setCapStyle(Qt::RoundCap);
    m_selectedPen.setBrush(m_penSelectedGradient);

    m_penGradient.setColorAt(0, QColor("#2a8d7c"));
    m_drawPen.setWidth(3);
    m_drawPen.setCapStyle(Qt::RoundCap);
    m_drawPen.setBrush(m_penGradient);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    m_line = new QGraphicsPathItem(this);
    registerSubitem(m_line);
    m_line->setBrush(Qt::NoBrush);
    m_line->setZValue(1);

    m_lineSelected = new QGraphicsPathItem(this);
    registerSubitem(m_lineSelected);
    m_lineSelected->setBrush(Qt::NoBrush);
    m_lineSelected->setZValue(0);
    m_lineSelected->hide();

    m_pArrowHeadPolygon = new QGraphicsPolygonItem(this);
    registerSubitem(m_pArrowHeadPolygon);

    VertexConnectionLine::setMainColor(QColor("#2a8d7c"));
    VertexConnectionLine::setSelectedColor(m_selectedPen.color());

    m_labelItem = new LabelItem(this);
    registerSubitem(m_labelItem);
    m_labelItem->setZValue(1);
    m_labelItem->setBackgroundColor(GraphCommon::DEFAULT_VERTEX_TEXT_BGR_COLOR);
}

VertexConnectionLine::~VertexConnectionLine()
{
    if (m_fromVertex) {
        m_fromVertex->unsubscribeConnectionFrom(this);
    }

    if (m_toVertex) {
        m_toVertex->unsubscribeConnectionTo(this);
    }
}

void VertexConnectionLine::setVertexFrom(VertexObject *pVertexFrom)
{
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

VertexObject *VertexConnectionLine::getVertexFrom() const
{
    return m_fromVertex;
}

void VertexConnectionLine::setVertexTo(VertexObject *pVertexTo)
{
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

VertexObject *VertexConnectionLine::getVertexTo() const
{
    return m_toVertex;
}

void VertexConnectionLine::setLine(const QLineF &line)
{
    m_straightLine = line;
    updatePolygon();
}

void VertexConnectionLine::setLine(const QPointF &p1, const QPointF &p2)
{
    setLine(QLineF(p1, p2));
}

QLineF VertexConnectionLine::getLine() const
{
    return m_straightLine;
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

void VertexConnectionLine::resetPositions()
{
    if (nullptr == m_toVertex) {
        return;
    }
    m_toVertex->updateConnectionLines();
}

void VertexConnectionLine::setMainColor(const QColor &penColor)
{
    ItemBase::setMainColor(penColor);

    m_penGradient.setColorAt(1, getMainColor());
    m_drawPen.setBrush(m_penGradient);
    m_line->setPen(m_drawPen);
    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    m_pArrowHeadPolygon->setPen(currentPen);
}

void VertexConnectionLine::setSelectedColor(const QColor &penColor)
{
    ItemBase::setSelectedColor(penColor);

    auto selectedPen = QPen(getSelectedColor(), 5);
    m_lineSelected->setPen(selectedPen);
    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    m_pArrowHeadPolygon->setPen(currentPen);
}

void VertexConnectionLine::setShortName(const QString &iText)
{
    m_labelItem->setShortName(iText);
    ItemBase::setShortName(iText);
}

void VertexConnectionLine::setArrowSize(qreal size)
{
    m_arrowSize = size;
}

qreal VertexConnectionLine::getArrowSize() const
{
    return m_arrowSize;
}

void VertexConnectionLine::updatePolygon()
{
    m_boundingRect = {};
    m_line->setPath(createLinePath());
    m_lineSelected->setPath(m_line->path());

    auto labelPos = m_line->boundingRect().center();
    labelPos.setX(labelPos.x() - m_labelItem->boundingRect().width());
    m_labelItem->setPos(labelPos);

    m_penSelectedGradient.setStart(m_straightLine.p1());
    m_penSelectedGradient.setFinalStop(m_straightLine.p2());
    m_selectedPen.setBrush(m_penSelectedGradient);

    m_penGradient.setStart(m_straightLine.p1());
    m_penGradient.setFinalStop(m_straightLine.p2());
    m_drawPen.setBrush(m_penGradient);

    bool isP1Lefter = m_straightLine.x2() > m_straightLine.x1();
    bool isP1Higher = m_straightLine.y2() > m_straightLine.y1();

    m_boundingRect.moveTop((isP1Higher ? m_straightLine.y1() : m_straightLine.y2()) - m_arrowSize);
    m_boundingRect.moveLeft((isP1Lefter ? m_straightLine.x1() : m_straightLine.x2()) - m_arrowSize);
    m_boundingRect.setWidth(std::fabs(m_straightLine.x2() - m_straightLine.x1()) + m_arrowSize * 2);
    m_boundingRect.setHeight(std::fabs(m_straightLine.y2() - m_straightLine.y1()) + m_arrowSize * 2);
}

QPainterPath VertexConnectionLine::createLinePath()
{
    auto pointFrom = m_straightLine.p1();
    pointFrom.setY(pointFrom.y() + m_arrowSize);

    auto pointTo = m_straightLine.p2();
    pointTo.setX(pointTo.x() + (pointFrom.x() > pointTo.x() ? m_arrowSize : - m_arrowSize));
    pointTo.setY(pointTo.y() - m_arrowSize);

    QPainterPath p;
    p.moveTo(m_straightLine.p1());
    p.lineTo(pointFrom);

    auto firstControlPoint = m_straightLine.center();
    firstControlPoint.setY(pointFrom.y());

    auto secondControlPoint = m_straightLine.center();
    secondControlPoint.setY(pointTo.y());

    p.cubicTo(pointFrom, firstControlPoint, m_straightLine.center());
    p.cubicTo(m_straightLine.center(), secondControlPoint, pointTo);

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

        p.lineTo(m_straightLine.p2());
        p.lineTo(arrowP2);
        p.lineTo(arrowP3);
        p.lineTo(arrowP1);
    }

    return p;
}

QPolygonF VertexConnectionLine::createPolygon(const QLineF &line)
{
    QRectF rect;
    QTransform transf;

    auto leftX      = line.x1() > line.x2() ? line.x1() : line.x2();
    auto bottomY    = line.y1() > line.y2() ? line.y2() : line.y1();

    rect.setLeft(leftX);
    rect.setBottom(bottomY);

    rect.setHeight(line.length());
    rect.setWidth(5);

    rect.moveTo(line.center().x() - leftX, line.center().y() - bottomY);

    transf.rotate(line.angle());

    return transf.mapRect(rect);
}

void VertexConnectionLine::updatePen()
{
    if (isSelected() == m_prevSelectedState) {
        return;
    }
    m_prevSelectedState = isSelected();

    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    m_pArrowHeadPolygon->setPen(currentPen);
}

QVariant VertexConnectionLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange) {
        if (value.toBool()) {
            m_lineSelected->show();
        } else {
            m_lineSelected->hide();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

QRectF VertexConnectionLine::boundingRect() const
{
    return m_boundingRect;
}

bool VertexConnectionLine::contains(const QPointF &p) const
{
    return m_pArrowHeadPolygon->contains(p) || m_line->contains(p);
}

QPainterPath VertexConnectionLine::shape() const
{
    QPainterPath res;
    res.addPath(m_line->shape());
    res.addPath(m_pArrowHeadPolygon->shape());
    return res;
}

}
