#ifndef VERTEXCONNECTIONLINE_H
#define VERTEXCONNECTIONLINE_H

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>

namespace PredefinedObjects
{

class VertexConnectionLine : public QGraphicsItem
{
public:
    explicit VertexConnectionLine(QGraphicsItem *parent = nullptr);

    void setLine(const QLineF& line);

    void setPositionFrom(const QPointF& posFrom);
    void setPositionTo(const QPointF& posTo);

    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    void setArrowSize(qreal size);
    qreal arrowSize() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*widget) override;

private:
    void updatePolygon();

    QPen   m_drawPen;
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QRectF m_boundingRect;

    std::list<QGraphicsLineItem*>   m_lines;
    QGraphicsPolygonItem*           m_pArrowHeadPolygon {nullptr};

    bool  m_prevSelectedState {false};
    qreal m_arrowSize {10};

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void updatePen();

public:
    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;
};

}

#endif // VERTEXCONNECTIONLINE_H
