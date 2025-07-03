#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

#include <QLabel>

namespace PredefinedObjects
{

class VertexObject : public QGraphicsItem
{
public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);

    void setImage(const QPixmap& pxmap);

    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    QRectF boundingRect() const override;
    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*widget) override;

private:
    void updatePolygon();

    QPen   m_drawPen;
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QRectF m_boundingRect;

    QLabel m_imageLabel;

    QGraphicsProxyWidget*   m_vertexImage   {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse {nullptr};
    QGraphicsTextItem*      m_vertexText    {nullptr};

    bool  m_prevSelectedState       {false};
    bool  m_isVertexContainPixmap   {false};

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void updatePen();
};

}

#endif // VERTEXOBJECT_H
