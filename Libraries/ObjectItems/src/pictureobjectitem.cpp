#include "pictureobjectitem.h"

#include <Common/CommonFunctions.h>
#include <Common/Logging.h>

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QTextOption>

#include <QImage>

#include "constants.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems {

PictureObjectItem::PictureObjectItem(QGraphicsItem* parent) : ItemBase(parent) {
    setSystemName("Изображение");

    setType(ObjectViewConstants::OBJECTTYPE_PICTURE);

    m_selectedRectItem = new QGraphicsPathItem(this);
    registerSubitem(m_selectedRectItem);
    m_selectedRectItem->hide();
    m_selectedRectItem->setZValue(1);

    m_vertexImage = new QGraphicsPixmapItem(this);
    registerSubitem(m_vertexImage);
    m_vertexImage->hide();

    m_vertexEllipse = new QGraphicsEllipseItem(this);
    registerSubitem(m_vertexEllipse);

    m_nameItem = new LabelItem(this);
    registerSubitem(m_nameItem);
    m_nameItem->setBorderColor(Qt::black);
    m_nameItem->setZValue(0);
}

PictureObjectItem::~PictureObjectItem() {}

LabelItem* PictureObjectItem::getLabel() const {
    return m_nameItem;
}

void PictureObjectItem::setImage(const QImage& img, const QString &imageHash) {
    if (img.isNull()) {
        setData(ObjectViewConstants::OBJECTFIELD_PICTURE_HASH, {});
        m_vertexImage->setPixmap({});
        m_vertexImage->hide();
        m_vertexEllipse->show();
        return;
    }

    setData(ObjectViewConstants::OBJECTFIELD_PICTURE_HASH, imageHash);

    auto newImage = QPixmap::fromImage(img);
//    newImage = newImage.scaled(m_vertexEllipse->boundingRect().width(),
//                               m_vertexEllipse->boundingRect().height());
    newImage = newImage.scaled(1000, 1000);
    m_vertexImage->setPixmap(newImage);
    m_vertexImage->show();
    m_vertexEllipse->hide();

    // Апдейт области
    setRect(boundingRect());
}

QString PictureObjectItem::getImageHash() const
{
    return data(ObjectViewConstants::OBJECTFIELD_PICTURE_HASH).toString();
}

void PictureObjectItem::setDisplayName(const QString& iText) {
    m_nameItem->setDisplayName(iText);
    ItemBase::setDisplayName(iText);

    // Апдейт области
    setRect(boundingRect());
}

void PictureObjectItem::setBorderColor(const QColor& penColor) {
    m_vertexEllipse->setPen(QPen(penColor, 5));
    ItemBase::setBorderColor(m_vertexEllipse->pen().color());
}

void PictureObjectItem::setBackgroundColor(const QColor& penColor) {
    m_vertexEllipse->setBrush(penColor);
    ItemBase::setBackgroundColor(m_vertexEllipse->brush().color());
}

void PictureObjectItem::setSelectionColor(const QColor& penColor) {
    ItemBase::setSelectionColor(penColor);

    auto selectedPen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);
    QRadialGradient gradient(0, 0, 100);
    gradient.setColorAt(0, QColor("#c5ffb3"));
    gradient.setColorAt(0.5, QColor("#a3ff8a"));
    gradient.setColorAt(1, getSelectionColor());
    selectedPen.setBrush(gradient);
    m_selectedRectItem->setPen(selectedPen);
}

QImage PictureObjectItem::getImage() const {
    return m_vertexImage->pixmap().toImage();
}

QRectF PictureObjectItem::getImageRect() const {
    return m_vertexEllipse->rect();
}

void PictureObjectItem::setRect(const QRectF& iRect) {
    const double selectionPadding = 20;

    // Задаю прямоугольник, чтобы boundingRect() подхватил область определения
    // вершины
    setBoundingRect(iRect);
    auto itemRect = iRect;
    itemRect.setX(0);
    itemRect.setY(0);
    itemRect.setWidth(iRect.width());
    itemRect.setHeight(iRect.height());

    // Изображение
    auto imageRect = itemRect;
    imageRect.setHeight(imageRect.height() - selectionPadding);
    m_vertexEllipse->setRect(imageRect);

    if (m_vertexImage->isVisible()) {
        m_vertexImage->setPixmap(m_vertexImage->pixmap().scaled(
            QSize(itemRect.width(), itemRect.height()),
            Qt::AspectRatioMode::KeepAspectRatio));
    }
    m_vertexImage->setPos(
        {(boundingRect().width() - m_vertexImage->boundingRect().width()) / 2.0,
         0});

    // Текст
    const double labelPadding = 5.0;  // Отступ для визуального разделения
    m_nameItem->setPos(0, boundingRect().height());

    // Выбор
    auto itemRoundRect = itemRect;
    itemRoundRect.moveTo(-selectionPadding / 2.0, -selectionPadding / 2.0);
    itemRoundRect.setWidth(itemRoundRect.width() + selectionPadding);
    itemRoundRect.setHeight(itemRoundRect.height() + selectionPadding +
                            m_nameItem->boundingRect().height());
    QPainterPath path;
    path.addRoundedRect(itemRoundRect, 10, 10);
    m_selectedRectItem->setPath(path);
}

QPainterPath PictureObjectItem::shape() const {
    QPainterPath res;

    if (m_vertexImage->isVisible()) {
        res.addPath(m_vertexImage->shape());
    }

    if (m_vertexEllipse->isVisible()) {
        res.addPath(m_vertexEllipse->shape());
    }

    res.addPath(m_nameItem->shape());
    return res;
}

QVariant PictureObjectItem::itemChange(GraphicsItemChange change,
                                       const QVariant& value) {
    if (change == ItemSelectedChange) {
        if (value.toBool()) [[unlikely]] {
            m_selectedRectItem->show();
        } else {
            m_selectedRectItem->hide();
        }
    }

    return ItemBase::itemChange(change, value);
}

}  // namespace ObjectViewItems
