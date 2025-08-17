#ifndef CENTERITEM_H
#define CENTERITEM_H

#include "itembase.h"

#include <QGraphicsEllipseItem>

namespace ObjectViewItems {

class CenterItem : public ItemBase {
public:
    CenterItem(ItemBase* parent = nullptr);
    ~CenterItem();

    QPainterPath shape() const override;

private:
    QSize m_centerSize {30, 30};

    QGraphicsEllipseItem*   m_pCenterEllipse {nullptr};

    QGraphicsEllipseItem*   m_pCenterTargetEllipse {nullptr};
    QGraphicsLineItem*      m_pCenterVerticalLine {nullptr};
    QGraphicsLineItem*      m_pCenterHorizontalLine {nullptr};
};

}  // namespace ObjectViewItems

#endif  // CENTERITEM_H
