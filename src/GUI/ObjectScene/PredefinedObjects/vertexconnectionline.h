#ifndef VERTEXCONNECTIONLINE_H
#define VERTEXCONNECTIONLINE_H

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>

namespace PredefinedObjects
{

class VertexObject;

class VertexConnectionLine : public QGraphicsItem
{
public:
    explicit VertexConnectionLine(QGraphicsItem *parent = nullptr);
    ~VertexConnectionLine();

    void setVertexFrom(VertexObject* pVertexFrom);
    void setVertexTo(VertexObject* pVertexTo);

    void setLine(const QLineF& line);
    QLineF getLine() const;

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
    VertexObject*   m_fromVertex {nullptr};
    VertexObject*   m_toVertex {nullptr};

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

    void updatePolygon();
    QPainterPath createLinePath();
    QPolygonF createPolygon(const QLineF& line);
    void updatePen();
};

}

#endif // VERTEXCONNECTIONLINE_H
