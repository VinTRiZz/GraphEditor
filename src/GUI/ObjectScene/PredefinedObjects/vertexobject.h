#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>

#include <QPen>

#include <QLabel>

#include <set>

namespace PredefinedObjects
{

class VertexConnectionLine;

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

    void setSelected(bool isItemSelected);

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    void updateConnectionLines();

private:
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QGraphicsPathItem*      m_selectedRectItem  {nullptr};
    QGraphicsPixmapItem*    m_vertexImage       {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse     {nullptr};
    QGraphicsRectItem*      m_vertexTextRect    {nullptr};
    QGraphicsTextItem*      m_vertexText        {nullptr};

    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void setupTextItem();
};

}

#endif // VERTEXOBJECT_H
