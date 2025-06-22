#ifndef VERTEXCONNECTIONLINE_H
#define VERTEXCONNECTIONLINE_H

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

namespace PredefinedObjects
{

class VertexConnectionLine : public QGraphicsItem
{
public:
    VertexConnectionLine(QGraphicsItem *parent = nullptr);

    void setPositionFrom(const QPointF& posFrom);
    void setPositionTo(const QPointF& posTo);

    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    void setArrowSize(qreal size);
    qreal arrowSize() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    void updatePolygon();
    void drawArrow(QPainter *painter);

    QPen    m_drawPen;
    QPen    m_selectedPen;
    QLineF m_straightLine;

    QLineF m_correctionVerticalLine;
    QLineF m_correctionHorizontalLine;

    QLineF m_firstLine;
    QLineF m_secondLine;
    QLineF m_thirdLine;

    qreal m_arrowSize {10}; // Размер стрелки

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
};

}

#endif // VERTEXCONNECTIONLINE_H
