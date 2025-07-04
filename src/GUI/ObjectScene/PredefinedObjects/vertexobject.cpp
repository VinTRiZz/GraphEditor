#include "vertexobject.h"

#include <QLabel>

#include <QGraphicsSceneMouseEvent>

#include "logging.h"


namespace PredefinedObjects
{


VertexObject::VertexObject(QGraphicsItem *parent) :
    QGraphicsRectItem(parent),
    m_drawPen {QPen(Qt::red)},
    m_selectedPen {QPen(Qt::yellow, 5, Qt::SolidLine, Qt::RoundCap)}
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);

    m_vertexImage   = new QGraphicsProxyWidget(this);
    m_vertexImage->setWidget(&m_imageLabel);
    m_imageLabel.hide();
    m_vertexImage->setZValue(0);

    m_vertexEllipse = new QGraphicsEllipseItem(this);
    m_vertexEllipse->setPen(QPen(Qt::darkGray));
    m_vertexEllipse->setBrush(QColor("#ecd1a6"));
    m_vertexEllipse->setZValue(0);

    m_vertexTextRect = new QGraphicsRectItem(this);
    m_vertexTextRect->setZValue(0);
    m_vertexTextRect->setBrush(Qt::white);

    m_vertexText    = new QGraphicsTextItem(this);
    m_vertexText->setDefaultTextColor(Qt::black);
    m_vertexText->setZValue(0);

    setPen(m_drawPen);
}

void VertexObject::setImage(const QPixmap &pxmap)
{
    m_imageLabel.setPixmap(pxmap);
    m_imageLabel.show();

    m_vertexEllipse->hide();
}

void VertexObject::setText(const QString &text)
{
    m_vertexText->setPlainText(text);
}

void VertexObject::setRect(const QRectF &iRect)
{
    QGraphicsRectItem::setRect(iRect);
    auto itemRect = iRect;
    itemRect.setX(0); itemRect.setY(0); itemRect.setWidth(iRect.width()); itemRect.setHeight(iRect.height());
    qDebug() << "Set rect of" << this << "as" << iRect;

    const double maxParts   = 5.0;
    const double imageParts = 4.0;
    const double textParts  = maxParts - imageParts;
    const double minSize    = 25.0;

    auto imageRect = itemRect;
    imageRect.setHeight(std::max(itemRect.height() * imageParts / maxParts, minSize * 2));
    m_vertexEllipse->setRect(imageRect);
    m_imageLabel.setGeometry(imageRect.toRect());

    auto textRect = itemRect;
    textRect.setHeight(std::max(itemRect.height() * textParts / maxParts, minSize));
    textRect.moveTo(0, imageRect.height() + imageRect.y());
    m_vertexTextRect->setRect(textRect);
    m_vertexText->setPos((textRect.width() - m_vertexText->boundingRect().width()) / 2, imageRect.height() - textRect.height() / 5);
}

void VertexObject::setPen(const QPen &pen)
{
    m_drawPen = pen;
    m_vertexText->setDefaultTextColor(pen.color());
}

void VertexObject::setSelectedPen(const QPen &pen)
{
    m_selectedPen = pen;
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

    return m_vertexTextRect->contains(p);
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

    res.addPath(m_vertexTextRect->shape());

    return res;
}

void VertexObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void VertexObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    auto isContain = contains(mapToScene(event->pos()));
    setSelected(isContain ^ isSelected());
    LOG_DEBUG("Selected:", isSelected());
    updatePen();
}

void VertexObject::updatePen()
{
    if (isSelected() == m_prevSelectedState) {
        return;
    }
    m_prevSelectedState = isSelected();

    auto currentPen = isSelected() ? m_selectedPen : m_drawPen;
    setPen(currentPen);
}

}
