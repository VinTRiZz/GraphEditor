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

    void setPen(const QColor &penColor);
    void setSelectedPen(const QColor &penColor);

    void setArrowSize(qreal size);
    qreal getArrowSize() const;

    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*widget) override;

private:
    void updatePolygon();
    QPainterPath createLinePath();
    QPolygonF createPolygon(const QLineF& line);

    QLinearGradient m_penSelectedGradient;
    QLinearGradient m_penGradient;
    QPen            m_drawPen;
    QPen            m_selectedPen;
    QLineF          m_straightLine;

    QRectF m_boundingRect;

    QGraphicsPathItem*      m_line              {nullptr};
    QGraphicsPathItem*      m_lineSelected      {nullptr};
    QGraphicsPolygonItem*   m_pArrowHeadPolygon {nullptr};

    bool  m_prevSelectedState {false};
    qreal m_arrowSize {8};

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void updatePen();
};

}

#endif // VERTEXCONNECTIONLINE_H
