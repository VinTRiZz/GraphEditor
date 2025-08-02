#include "informationobjectview.h"

#include <QMouseEvent>

InformationObjectView::InformationObjectView(QWidget *parent) :
    InteractiveObjectView(parent)
{
    m_pCoordinatesItem = new ObjectViewItems::LabelItem();
    m_pCoordinatesItem->setBackgroundColor(QColor(200, 240, 210, 80));
    m_pCoordinatesItem->setMainColor(Qt::black);
    m_pCoordinatesItem->setTextSize(10);
    m_pCoordinatesItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    m_pCoordinatesItem->setZValue(1e6); // Чтобы точно не закрыло
    scene()->addItem(m_pCoordinatesItem);
}

void InformationObjectView::mouseMoveEvent(QMouseEvent *e)
{
    auto currentPos = mapToScene(e->pos());

    m_pCoordinatesItem->setPos(mapToScene({e->pos().x() + 20, e->pos().y() + 20}));

    QString hoverItemName {};
    auto hoverItem = itemAt(e->pos());
    if (nullptr != hoverItem) {
        auto pHoverItemParent = getParentOfComplex(hoverItem);
        if (nullptr != pHoverItemParent) {
            hoverItemName = pHoverItemParent->getSystemName();
        } else {
            hoverItemName = hoverItem->data(ObjectViewConstants::OBJECTFIELD_NAME_SYSTEM).toString();
        }
    }
    m_pCoordinatesItem->setShortName(
        QString("X: %0\nY: %1%2").arg(
            QString::number(currentPos.x()),
            QString::number(currentPos.y()),
            !hoverItemName.isEmpty() ? QString("\n") + hoverItemName : QString())
    );
    InteractiveObjectView::mouseMoveEvent(e);
}
