#include "groupitem.hpp"

#include <QPainter>
#include <Components/Maths/Graphical.h>
#include <Components/Logger/Logger.h>

namespace ObjectItems {

GroupItem::GroupItem(QGraphicsItem* parent)
    : BasicItem(parent)
{
    setSystemName("Group");
    setObjectType(ObjectType::OIT_GroupItem);

    createSubitem(m_bPolygon);
    m_bPolygon->setZValue(-1);
    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_bPolygon->setPen(getLinePen());
        m_bPolygon->setBrush(getBackgroundBrush());
    });

    connect(this, &BasicItem::itemAboutToMove,
            this, [this](auto curPos){
        if (nullptr != parentItem()) {
            curPos = parentItem()->mapToScene(curPos);
        }
        auto deltaPos = scenePos() - curPos;
        for (auto* pItem : m_groupItems) {
            auto itemDeltaPos = deltaPos;
            if (nullptr != pItem->parentItem()) {
                itemDeltaPos = pItem->parentItem()->mapFromScene(itemDeltaPos);
            }
            pItem->setPos(pItem->pos() - itemDeltaPos);
        }
        updateBoundingPolygon();
    });
}

void GroupItem::setGroupItems(const QList<BasicItem*>& items)
{
    QList<BasicItem*> diffItems;
    std::set_difference(items.begin(), items.end(),
                        m_groupItems.begin(), m_groupItems.end(),
                        std::back_inserter(diffItems));
    for (auto* pItem : diffItems) {
        connect(pItem, &BasicItem::itemMovedOnScene,
                   this, &GroupItem::updateBoundingPolygon);
    }
    m_groupItems = items;
    updateBoundingPolygon();
}

void GroupItem::addGroupItem(BasicItem* item)
{
    if (item && !m_groupItems.contains(item)) {
        m_groupItems.append(item);
        connect(item, &BasicItem::itemMovedOnScene,
                   this, &GroupItem::updateBoundingPolygon);
        connect(item, &QObject::destroyed,
                this, [this, item](){
            removeGroupItem(item);
        });
        updateBoundingPolygon();
    }
}

void GroupItem::removeGroupItem(BasicItem *item)
{
    disconnect(item, &BasicItem::itemMovedOnScene,
               this, nullptr);
    m_groupItems.removeOne(item);
    updateBoundingPolygon();
    if (m_groupItems.size() == 0 &&
        m_deleteOnEmpty) {
        delete this;
    }
}

void GroupItem::enableDeleteOnEmpty()
{
    m_deleteOnEmpty = true;
}

QPolygonF GroupItem::getBoundingPolygon() const
{
    return m_bPolygon->polygon();
}

void GroupItem::updateBoundingPolygon()
{
    std::vector<Maths::Point> points;
    const auto offsetTopRPoint = QPointF(20, -20);
    const auto offsetTopLPoint = QPointF(-20, -20);
    const auto offsetButtRPoint = QPointF(20, 20);
    const auto offsetButtLPoint = QPointF(-20, 20);
    for (auto* pItem : m_groupItems) {

        // Для выпуклой оболочки
        auto bRect = pItem->sceneBoundingRect();

        points.push_back(bRect.topLeft() + offsetTopLPoint);
        points.push_back(bRect.topRight() + offsetTopRPoint);
        points.push_back(bRect.bottomLeft() + offsetButtLPoint);
        points.push_back(bRect.bottomRight() + offsetButtRPoint);
    }

    auto thisParent = parentItem();
    if (thisParent) {
        for (auto& p : points) {
            p = thisParent->mapFromScene(p);
        }
    }
    auto roundPoly = Maths::Graphical::createOuterPolygon(points);
    QPolygonF boundingPoly;
    roundPoly.reserve(roundPoly.size());
    for (auto& p : roundPoly) {
        boundingPoly.push_back(p);
    }
    boundingPoly.translate(-scenePos());

    m_bPolygon->setPolygon(boundingPoly);
    emit graphicalDataChanged();
}


} // namespace ObjectItems
