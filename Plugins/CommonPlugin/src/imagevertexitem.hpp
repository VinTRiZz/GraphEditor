#pragma once

#include <GraphObject/GraphObject.h>

#include <QGraphicsPixmapItem>

namespace Graph {

class ImageVertexItem : public GObjectItem
{
    Q_OBJECT
public:
    explicit ImageVertexItem(QGraphicsItem* parent = nullptr);
    ~ImageVertexItem();

    void setImage(const QImage& pixmap);
    QImage getImage() const;

    void setAspectRatioMode(Qt::AspectRatioMode mode);
    Qt::AspectRatioMode getAspectRatioMode() const;

    virtual QMenu *createContextMenu() override;

    virtual QJsonObject toJson() const override;
    virtual bool fromJson(const QJsonObject& arr) override;

protected:
    void processSizeTypeChange(const QRectF& newSize) override;

private:
    void updatePixmap();

    QGraphicsPixmapItem* m_pixmapItem{nullptr};
    QImage m_originalImage;
    Qt::AspectRatioMode m_aspectRatioMode{Qt::KeepAspectRatio};
};

} // namespace Graph

