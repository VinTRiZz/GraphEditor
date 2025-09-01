#ifndef PICTUREOBJECTITEM_H
#define PICTUREOBJECTITEM_H

#include "itembase.h"
#include "labelitem.h"

#include <QGraphicsItem>
#include <QPen>
#include <set>

namespace ObjectViewItems {

class PictureObjectItem : public ItemBase {
public:
    explicit PictureObjectItem(QGraphicsItem* parent = nullptr);
    ~PictureObjectItem();

    LabelItem* getLabel() const;

    void setImage(const QImage& img);
    void setShortName(const QString& iText) override;
    void setName(const QString& iText) override;

    void setMainColor(const QColor& penColor) override;
    void setSecondColor(const QColor& penColor) override;
    void setSelectedColor(const QColor& penColor) override;

    QImage getImage() const;
    QRectF getImageRect() const;

    void setRect(const QRectF& iRect);

    QPainterPath shape() const override;

private:
    QGraphicsPathItem* m_selectedRectItem{nullptr};
    QGraphicsPixmapItem* m_vertexImage{nullptr};
    QGraphicsEllipseItem* m_vertexEllipse{nullptr};
    LabelItem* m_nameItem{nullptr};

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}  // namespace ObjectViewItems

#endif  // PICTUREOBJECTITEM_H
