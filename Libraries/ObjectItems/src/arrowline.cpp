#include "arrowline.h"

#include <math.h>

#include <QGraphicsLineItem>
#include <QPainter>
#include <QPen>

namespace ObjectViewItems {

ArrowedLine::ArrowedLine(QGraphicsItem *parent) : ItemBase(parent) {
  setSystemName("Соединение");
  setType(ObjectViewConstants::OBJECTTYPE_ARROWLINE);
  m_line = new QGraphicsLineItem(this);
  registerSubitem(m_line);
}

void ArrowedLine::setPen(const QPen &drawPen) { m_drawPen = drawPen; }

void ArrowedLine::setSelectedPen(const QPen &drawPen) {
  m_drawSelectedPen = drawPen;
}

void ArrowedLine::setLine(const QLineF &iLine) { m_line->setLine(iLine); }

void ArrowedLine::setLine(const QPointF &p1, const QPointF &p2) {
  setLine(QLineF(p1, p2));
}

QLineF ArrowedLine::getLine() const { return m_line->line(); }

void ArrowedLine::setArrowSize(qreal arrowSize) { m_arrowSize = arrowSize; }

qreal ArrowedLine::getArrowSize() const { return m_arrowSize; }

void ArrowedLine::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {
  drawArrow(painter);
}

void ArrowedLine::drawArrow(QPainter *painter) {
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
  QPointF arrowP3 =
      line.p2() + QPointF(sin(angle + PI_2_DELIM_3) * m_arrowSize,
                          cos(angle + PI_2_DELIM_3) * m_arrowSize);

  // Создаем полигон стрелки
  QPolygonF arrowHead;
  arrowHead << arrowP1 << arrowP2 << arrowP3;

  // Рисуем стрелку
  painter->setBrush(painter->pen().color());
  painter->drawPolygon(arrowHead);
}

QVariant ArrowedLine::itemChange(GraphicsItemChange change,
                                 const QVariant &value) {
  if (change == ItemSelectedChange) {
    if (value.toBool()) {
      m_line->setPen(m_drawSelectedPen);
    } else {
      m_line->setPen(m_drawPen);
    }
  }

  return QGraphicsItem::itemChange(change, value);
}

} // namespace ObjectViewItems
