#include "debugmaster.hpp"

#include <QBrush>
#include <QPen>

namespace ObjectItems {

DebugMaster::DebugMaster()
{

}

DebugMaster::DebugMaster(QGraphicsItem *pTargetItem)
{
    init(pTargetItem);
}

DebugMaster::~DebugMaster()
{

}

void DebugMaster::init(QGraphicsItem *pTargetItem)
{
    m_targetItem = pTargetItem;

    m_debugRectItem = new QGraphicsRectItem(pTargetItem);
    m_debugRectItem->setZValue(1'000'000);
    m_debugRectItem->setBrush(QBrush(Qt::magenta, Qt::DiagCrossPattern));
    m_debugRectItem->setPen(QPen(Qt::red, 2, Qt::DotLine));
    m_debugRectItem->hide();

    m_debugCenterCircle = new QGraphicsEllipseItem(pTargetItem);
    m_debugCenterCircle->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    m_debugCenterCircle->setZValue(1'001'000);
    m_debugCenterCircle->setBrush(QBrush(Qt::darkMagenta, Qt::DiagCrossPattern));
    m_debugCenterCircle->setPen(QPen(Qt::green, 4, Qt::SolidLine));
    m_debugCenterCircle->setRect(QRectF(-30, -30, 30, 30));
    m_debugCenterCircle->hide();

    m_debugCustomRectItem = new QGraphicsRectItem(pTargetItem);
    m_debugCustomRectItem->setZValue(1'000'001);
    m_debugCustomRectItem->setBrush(QBrush(Qt::darkCyan, Qt::BDiagPattern));
    m_debugCustomRectItem->setPen(QPen(Qt::darkCyan, 3, Qt::DashDotDotLine));
    m_debugCustomRectItem->hide();
}

QGraphicsItem *DebugMaster::getTargetItem() const
{
    return m_targetItem;
}

void DebugMaster::setCustomRectVisible(const QRectF &rect, bool isCRectVisible)
{
    m_debugCustomRectItem->setRect(rect);
    if (isCRectVisible) {
        m_debugCustomRectItem->show();
    } else {
        m_debugCustomRectItem->setRect({});
        m_debugCustomRectItem->hide();
    }
}

void DebugMaster::setCenterVisible(bool isCenterVisible)
{
    if (isCenterVisible) {
        m_debugCenterCircle->show();
    } else {
        m_debugCenterCircle->hide();
    }
}

void DebugMaster::setBoundingRectVisible(bool isBRectVisible)
{
    if (isBRectVisible) {
        m_debugRectItem->setRect(m_targetItem->boundingRect());
        m_debugRectItem->show();
    } else {
        m_debugRectItem->setRect({});
        m_debugRectItem->hide();
    }
}

void DebugMaster::showCenter(QGraphicsItem *pItem)
{
    DebugMaster mst(pItem);
    mst.setCenterVisible(true);
}

void DebugMaster::showBoundingRect(QGraphicsItem *pItem)
{
    DebugMaster mst(pItem);
    mst.setBoundingRectVisible(true);
}


} // namespace ObjectItems
