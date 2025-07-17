#include "arrowline.h"

#include <QGraphicsLineItem>
#include <QPainter>
#include <QPen>

#include <math.h>

namespace PredefinedObjects
{

ArrowedLine::ArrowedLine(QGraphicsItem *parent)
    : PredefinedObjectBase(parent) {
    setType(ObjectSceneConstants::OBJECTTYPE_ARROWLINE);
}

void ArrowedLine::setLine(const QLineF &iLine)
{
    m_straightLine = iLine;
}

QLineF ArrowedLine::getLine() const
{
    return m_straightLine;
}

void ArrowedLine::setArrowSize(qreal arrowSize) { m_arrowSize = arrowSize; }

qreal ArrowedLine::getArrowSize() const { return m_arrowSize; }

void ArrowedLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawLine(m_straightLine);
    drawArrow(painter);
}

void ArrowedLine::drawArrow(QPainter *painter)
{
    QLineF line = getLine();
    if (line.length() == 0)
        return;

    // Угол линии
    double angle = (getLine().angle() + 180) * M_PI / 180.0;

    static auto PI_DELIM_3 = M_PI / 3;
    static auto PI_2_DELIM_3 = M_PI * 2 / 3;

    // Точка конца линии (куда будет направлена стрелка)
    QPointF arrowP1 = line.p2();

    // Первая точка "крыла" стрелки
    QPointF arrowP2 = line.p2() + QPointF(sin(angle + PI_DELIM_3) * m_arrowSize,
                                          cos(angle + PI_DELIM_3) * m_arrowSize);

    // Вторая точка "крыла" стрелки
    QPointF arrowP3 = line.p2() + QPointF(sin(angle + PI_2_DELIM_3) * m_arrowSize,
                                          cos(angle + PI_2_DELIM_3) * m_arrowSize);

    // Создаем полигон стрелки
    QPolygonF arrowHead;
    arrowHead << arrowP1 << arrowP2 << arrowP3;

    // Рисуем стрелку
    painter->setBrush(painter->pen().color());
    painter->drawPolygon(arrowHead);
}

}
