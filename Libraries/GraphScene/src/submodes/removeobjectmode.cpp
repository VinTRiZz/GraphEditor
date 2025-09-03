#include "removeobjectmode.h"

#include "graphsceneview.h"

namespace Graph {

RemoveObjectMode::RemoveObjectMode(GraphModeBase* pParentMode)
    : GraphSubmodeBase(pParentMode) {
    auto& buttonConf = getStarterButton();
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_remove.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_remove_active.svg");
    buttonConf.tooltip = "Удаление элементов графа";
    buttonConf.positionX = -1;
    buttonConf.positionY = 0;
}

void RemoveObjectMode::clearMode() {}

void RemoveObjectMode::processPress(QGraphicsItem* pTargetItem) {
    auto pItem = dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem);
    if (nullptr == pItem) {
        return;
    }
    getParentMode()->getScene()->removeObject(pItem->getObjectId());
}

void RemoveObjectMode::processMove(QGraphicsItem* pTargetItem,
                                   const QPointF& currentPos) {}

void RemoveObjectMode::processRelease(QGraphicsItem* pTargetItem) {}

}  // namespace Graph
