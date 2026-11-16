#pragma once

#include "basicitem.hpp"

namespace ObjectItems {

class GroupItem : public BasicItem
{
    Q_OBJECT
public:
    explicit GroupItem(QGraphicsItem* parent = nullptr);

    // Установка комментируемых элементов
    void setGroupItems(const QList<BasicItem*>& items);
    void addGroupItem(BasicItem* item);
    void removeGroupItem(BasicItem* item);

    void enableDeleteOnEmpty();

    QPolygonF getBoundingPolygon() const;

private:
    void updateBoundingPolygon();

    bool m_deleteOnEmpty {false};

    QGraphicsPolygonItem* m_bPolygon {nullptr};
    QList<BasicItem*> m_groupItems;

    QPointF m_prevScenePos;
};

} // namespace ObjectItems

