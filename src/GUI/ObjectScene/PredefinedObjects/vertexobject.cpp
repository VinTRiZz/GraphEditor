#include "vertexobject.h"

#include <QLabel>


namespace PredefinedObjects
{


VertexObject::VertexObject(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    m_vertexImage = new QGraphicsProxyWidget(this);
    m_vertexImage->setWidget(&m_imageLabel);
    m_imageLabel.hide();
    m_vertexEllipse = new QGraphicsEllipseItem(this);
    m_vertexText  = new QGraphicsTextItem(this);
}

void VertexObject::setImage(const QPixmap &pxmap)
{
    m_imageLabel.setPixmap(pxmap);
    m_imageLabel.show();
}

void VertexObject::setPen(const QPen &pen)
{
    m_drawPen = pen;
}

void VertexObject::setSelectedPen(const QPen &pen)
{
    m_selectedPen = pen;
}

QRectF VertexObject::boundingRect() const
{
    return m_boundingRect;
}

bool VertexObject::contains(const QPointF &p) const
{
    if (m_vertexImage->isVisible()) {
        if (m_vertexImage->contains(p)) {
            return true;
        }
    }

    if (m_vertexEllipse->isVisible()) {
        if (m_vertexEllipse->contains(p)) {
            return true;
        }
    }

    return m_vertexText->contains(p);
}

QPainterPath VertexObject::shape() const
{
    QPainterPath res;

    if (m_vertexImage->isVisible()) {
        res.addPath(m_vertexImage->shape());
    }

    if (m_vertexEllipse->isVisible()) {
        res.addPath(m_vertexEllipse->shape());
    }

    res.addPath(m_vertexText->shape());

    return res;
}

void VertexObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

}

void VertexObject::updatePolygon()
{

}

void VertexObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void VertexObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

void VertexObject::updatePen()
{

}

}
