#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

#include <QLabel>

namespace PredefinedObjects
{

class VertexObject : public QGraphicsRectItem
{
    // Запрещаем, т.к. надо чтобы было прозрачно.
    using QGraphicsRectItem::setBrush;

public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);

    void setImage(const QPixmap& pxmap);
    void setText(const QString& text);

    void setRect(const QRectF& iRect);

    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    bool contains(const QPointF& p) const override;
    QPainterPath shape() const override;

private:
    void updatePolygon();

    QPen   m_drawPen;
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QLabel m_imageLabel;

    QGraphicsProxyWidget*   m_vertexImage       {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse     {nullptr};
    QGraphicsRectItem*      m_vertexTextRect    {nullptr};
    QGraphicsTextItem*      m_vertexText        {nullptr};

    bool  m_prevSelectedState       {false};
    bool  m_isVertexContainPixmap   {false};

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void updatePen();
};

}

#endif // VERTEXOBJECT_H
