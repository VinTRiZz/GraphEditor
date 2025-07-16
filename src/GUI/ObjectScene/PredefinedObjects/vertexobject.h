#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>

#include <QPen>

#include <QLabel>

namespace PredefinedObjects
{

class VertexObject : public QGraphicsRectItem
{
    // Запрещаем, т.к. надо чтобы было прозрачно.
    using QGraphicsRectItem::setBrush;

public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);

    void setImage(const QImage& img);

    void setText(const QString& text);

    void setNodeColor(const QColor& borderColor, const QBrush &backgroundBrush);
    void setVertexBrush(const QBrush& mainBackground, const QBrush& textBackground);

    void setRect(const QRectF& iRect);

    QPainterPath shape() const override;

private:
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QGraphicsPathItem*      m_selectedRectItem  {nullptr};
    QGraphicsPixmapItem*    m_vertexImage       {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse     {nullptr};
    QGraphicsRectItem*      m_vertexTextRect    {nullptr};
    QGraphicsTextItem*      m_vertexText        {nullptr};

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void setupTextItem();
};

}

#endif // VERTEXOBJECT_H
