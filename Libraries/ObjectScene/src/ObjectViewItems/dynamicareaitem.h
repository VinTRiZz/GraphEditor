#ifndef DYNAMICAREAITEM_H
#define DYNAMICAREAITEM_H

#include "itembase.h"

#include <QBrush>
#include <QPen>

namespace ObjectViewItems
{

class DynamicAreaItem : public ItemBase
{
public:
    explicit DynamicAreaItem(QGraphicsItem* parent = nullptr);

    void setRectSize(const QRectF& iRect);
    void setBrush(const QBrush& iBrush);
    void setPen(const QPen& iPen);

    void registerItem(ItemBase* pItem);
    std::list<ItemBase*> getRegisteredItems() const;
    void clearRegisteredItems();

    void removeRegisteredItems(ObjectViewConstants::ObjectType objT);
    void removeRegisteredItem(ItemBase* pItem);

    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

private:

    // TODO: Убрать, когда напишу динамический ресайз
    QGraphicsRectItem* m_hardcodeRect {nullptr};


    QRectF m_fieldRect;
    QBrush m_fieldBrush;
    QPen   m_gridPen;

    std::pair<double, double> m_matrixMin; // Для маппинга
    std::pair<double, double> m_matrixMax; // Для маппинга

    std::list<std::list<QGraphicsRectItem*> > m_fieldMatrix; // cols with rows
    std::list<ItemBase*> m_registeredItems;

    void setFieldRect(int minX, int minY, int maxX, int maxY);
    void updateFieldRect();
    void updateRectSizes();
    QGraphicsItem* getFieldByPos(const QPointF& p);
};

}

#endif // DYNAMICAREAITEM_H
