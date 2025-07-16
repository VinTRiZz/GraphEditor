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

    void setVertexBrush(const QBrush& mainBackground, const QBrush& textBackground);

    void setRect(const QRectF& iRect);

    void setSelectedPen(const QPen& pen);

    QPainterPath shape() const override;

private:
    QPen   m_drawPen;
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QBrush m_backgroundMain {QColor("#ecd1a6")};
    QBrush m_backgroundText {QColor("#f3f5dd")};

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
