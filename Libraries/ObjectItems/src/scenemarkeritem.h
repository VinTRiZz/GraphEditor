#ifndef SCENEMARKERITEM_H
#define SCENEMARKERITEM_H

#include "itembase.h"

namespace ObjectViewItems
{

class SceneMarkerItem : public ItemBase
{
public:
    explicit SceneMarkerItem(ItemBase* parent = nullptr);

    void setTarget(QGraphicsItem* pTarget);
    QGraphicsItem* getTarget() const;

    void setTargetIcon(const QImage& targetImage);

private:
    QGraphicsItem*  m_pTarget {nullptr};

    QPainterPath    m_objectFigurePath;
    QImage          m_targetImage;

    QSize           m_markerSize {30, 30};

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

}

#endif // SCENEMARKERITEM_H
