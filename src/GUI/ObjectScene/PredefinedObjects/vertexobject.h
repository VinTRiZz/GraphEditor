#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>


namespace PredefinedObjects
{

class VertexObject : public QGraphicsItem
{
public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);

    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    QRectF boundingRect() const override;
    QPainterPath opaqueArea() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
};

}

#endif // VERTEXOBJECT_H
