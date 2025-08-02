#ifndef VERTEXCONNECTIONLINE_H
#define VERTEXCONNECTIONLINE_H

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>

#include "itembase.h"
#include "labelitem.h"

namespace ObjectViewItems
{

class VertexObject;

class VertexConnectionLine : public ItemBase
{
public:
    explicit VertexConnectionLine(QGraphicsItem *parent = nullptr);
    ~VertexConnectionLine();

    void setVertexFrom(VertexObject* pVertexFrom);
    VertexObject* getVertexFrom() const;

    void setVertexTo(VertexObject* pVertexTo);
    VertexObject* getVertexTo() const;

    void setLine(const QLineF& line);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    void setPositionFrom(const QPointF& posFrom);
    void setPositionTo(const QPointF& posTo);
    void resetPositions();

    void setMainColor(const QColor &penColor) override;
    void setSelectedColor(const QColor &penColor) override;

    void setShortName(const QString& iText) override;

    void setArrowSize(qreal size);
    qreal getArrowSize() const;

    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

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
    LabelItem*              m_labelItem         {nullptr};

    bool  m_prevSelectedState {false};
    qreal m_arrowSize {8};

    void updatePolygon();
    QPainterPath createLinePath();
    QPolygonF createPolygon(const QLineF& line);
    void updatePen();

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

}

#endif // VERTEXCONNECTIONLINE_H
