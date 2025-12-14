#include "imagevertexitem.hpp"

#include <Components/Common/CommonFunctions.h>

#include <QFileDialog>

namespace Graph {

ImageVertexItem::ImageVertexItem(QGraphicsItem* parent)
    : VertexItem(parent)
{
    setSystemName("Изображение");

    createSubitem(m_pixmapItem);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
}

ImageVertexItem::~ImageVertexItem()
{
}

void ImageVertexItem::setImage(const QImage& pixmap)
{
    m_originalImage = pixmap;
    updatePixmap();
}

QImage ImageVertexItem::getImage() const
{
    return m_originalImage;
}

void ImageVertexItem::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    if (m_aspectRatioMode != mode) {
        m_aspectRatioMode = mode;
        updatePixmap();
    }
}

Qt::AspectRatioMode ImageVertexItem::getAspectRatioMode() const
{
    return m_aspectRatioMode;
}

QMenu* ImageVertexItem::createContextMenu()
{
    auto res = VertexItem::createContextMenu();

    auto pSetImageAction = new QAction("Выбрать изображение");
    connect(pSetImageAction, &QAction::triggered,
            this, [this](){
        QStringList mimeTypes;
        foreach (const QByteArray &format, QImageReader::supportedMimeTypes()) {
            mimeTypes.append(format);
        }

        auto targetFile = QFileDialog::getOpenFileName(nullptr,
                                           "Выбор изображения",
                                           "",
                                           QString("Поддерживаемые изображения (%1)").arg(mimeTypes.join(" ")));
        if (targetFile.isEmpty()) { // canceled
            return;
        }

        setImage(CommonFunctions::readImage(targetFile));
    });
    res->addAction(pSetImageAction);

    return res;
}

void ImageVertexItem::processSizeTypeChange(const QRectF& newSize)
{
    updatePixmap();
}

void ImageVertexItem::updatePixmap()
{
    if (m_originalImage.isNull()) {
        m_pixmapItem->setPixmap(QPixmap());
        return;
    }

    QRectF boundingRect = toVertexBoundingRect(getVertexSizeType());
    QSize targetSize = boundingRect.size().toSize();

    QPixmap scaledPixmap = QPixmap::fromImage(m_originalImage).scaled(
        targetSize,
        m_aspectRatioMode,
        Qt::SmoothTransformation
    );

    m_pixmapItem->setPixmap(scaledPixmap);

    // Центрируем изображение в boundingRect
    QPointF centerPos(
        boundingRect.x() + (boundingRect.width() - scaledPixmap.width()) / 2,
        boundingRect.y() + (boundingRect.height() - scaledPixmap.height()) / 2
    );
    m_pixmapItem->setPos(centerPos);
}

} // namespace Graph
