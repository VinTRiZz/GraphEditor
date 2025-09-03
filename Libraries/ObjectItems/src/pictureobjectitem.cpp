#include "pictureobjectitem.h"

#include <Common/ApplicationSettings.h>
#include <Common/Logging.h>
#include <GraphObject/Object.h>

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QImageReader>
#include <QLabel>
#include <QTextOption>

#include "constants.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems {

// Загрузка изображения с поддержкой прозрачности
QPixmap loadImageWithAlpha(const QString& path) {
    QImageReader reader(path);
    reader.setAutoTransform(true);  // Автоповорот по EXIF
    reader.setDecideFormatFromContent(
        true);  // Определение формата по содержимому

    if (reader.supportsAnimation()) {  // Для GIF/APNG
        return QPixmap::fromImage(reader.read());
    }
    return QPixmap(path);  // Для PNG/JPEG/BMP/etc
}

QString rectToString(const QRectF& iRect) {
    QString res;

    res = QString("%0:%1:%2:%3")
              .arg(QString::number(iRect.left()), QString::number(iRect.top()),
                   QString::number(iRect.width()),
                   QString::number(iRect.height()));

    return res;
}

QRectF rectFromString(const QString& iString) {
    auto valuesSplitted = iString.split(":");
    if (valuesSplitted.count() < 4) {
        LOG_WARNING("Invalid format of rect save:", iString);
        return {};
    }

    QRectF res;
    res.setTopLeft(
        QPointF(valuesSplitted[0].toDouble(), valuesSplitted[1].toDouble()));
    res.setWidth(valuesSplitted[2].toDouble());
    res.setHeight(valuesSplitted[3].toDouble());
    return res;
}

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

    auto& appSettings = ApplicationSettings::getInstance();

    PictureObjectItem::setSelectedColor(
        appSettings.getObjectsConfig().getNodeSelectionColor());
    PictureObjectItem::setSecondColor(
        appSettings.getObjectsConfig().getNodeSecondColor());
    PictureObjectItem::setMainColor(
        appSettings.getObjectsConfig().getNodeMainColor());

    m_nameItem = new LabelItem(this);
    registerSubitem(m_nameItem);
    m_nameItem->setSecondColor(
        appSettings.getObjectsConfig().getLabelBackgroundColor());
    m_nameItem->setMainColor(
        appSettings.getObjectsConfig().getLabelTextColor());
    m_nameItem->setZValue(0);
}

PictureObjectItem::~PictureObjectItem() {}

LabelItem* PictureObjectItem::getLabel() const {
    return m_nameItem;
}

void PictureObjectItem::setImage(const QImage& img) {
    if (img.isNull()) {
        return;
    }

    auto newImage = QPixmap::fromImage(img);
    newImage = newImage.scaled(m_vertexEllipse->boundingRect().width(),
                               m_vertexEllipse->boundingRect().height());
    m_vertexImage->setPixmap(newImage);
    m_vertexImage->show();
    m_vertexEllipse->hide();

    // Апдейт области
    setRect(boundingRect());
}

void PictureObjectItem::setShortName(const QString& iText) {
    m_nameItem->setShortName(iText);
    ItemBase::setShortName(iText);

    // Апдейт области
    setRect(boundingRect());
}

void PictureObjectItem::setName(const QString& iText) {
    m_nameItem->setName(iText);
    ItemBase::setName(iText);
}

void PictureObjectItem::setMainColor(const QColor& penColor) {
    if (penColor.isValid()) {
        m_vertexEllipse->setPen(QPen(penColor, 5));
    } else {
        auto& appSettings = ApplicationSettings::getInstance();
        m_vertexEllipse->setPen(
            QPen(appSettings.getObjectsConfig().getNodeMainColor(), 5));
    }
    ItemBase::setMainColor(m_vertexEllipse->pen().color());
}

void PictureObjectItem::setSecondColor(const QColor& penColor) {
    if (penColor.isValid()) {
        m_vertexEllipse->setBrush(penColor);
    } else {
        auto& appSettings = ApplicationSettings::getInstance();
        m_vertexEllipse->setBrush(
            appSettings.getObjectsConfig().getNodeSecondColor());
    }
    ItemBase::setSecondColor(m_vertexEllipse->brush().color());
}

void PictureObjectItem::setSelectedColor(const QColor& penColor) {
    ItemBase::setSelectedColor(penColor);

    auto selectedPen = QPen(Qt::black, 4, Qt::SolidLine, Qt::RoundCap);
    QRadialGradient gradient(0, 0, 100);
    gradient.setColorAt(0, QColor("#c5ffb3"));
    gradient.setColorAt(0.5, QColor("#a3ff8a"));
    gradient.setColorAt(1, getSelectedColor());
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
