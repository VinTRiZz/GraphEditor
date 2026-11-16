#pragma once

#include <QGraphicsItem>

namespace ObjectItems {

class DebugMaster {
    QGraphicsItem* m_targetItem {nullptr};
public:
    DebugMaster();
    explicit DebugMaster(QGraphicsItem* pTargetItem);
    ~DebugMaster();

    void init(QGraphicsItem* pTargetItem);

    QGraphicsItem* getTargetItem() const;

    void setCustomRectVisible(const QRectF& rect, bool isCRectVisible = true);
    void setCenterVisible(bool isCenterVisible = true);
    void setBoundingRectVisible(bool isBRectVisible = true);

    static void showCenter(QGraphicsItem* pItem);
    static void showBoundingRect(QGraphicsItem* pItem);

private:
    QGraphicsRectItem*      m_debugCustomRectItem {nullptr};
    QGraphicsRectItem*      m_debugRectItem {nullptr};
    QGraphicsEllipseItem*   m_debugCenterCircle {nullptr};
};

} // namespace ObjectItems

