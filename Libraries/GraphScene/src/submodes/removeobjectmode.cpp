#include "removeobjectmode.h"

#include "graphsceneview.h"

namespace Graph
{

void RemoveObjectMode::clearMode()
{

}

void RemoveObjectMode::processPress(QGraphicsItem *pTargetItem)
{
    auto pItem = dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem);
    if (nullptr == pItem) {
      return;
    }
    getParentMode()->getScene()->removeObject(pItem->getObjectId());
}

void RemoveObjectMode::processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos)
{

}

void RemoveObjectMode::processRelease(QGraphicsItem *pTargetItem)
{

}

ButtonMatrix::ButtonConfig RemoveObjectMode::getStarterButton()
{
    ButtonMatrix::ButtonConfig buttonConf;
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_remove.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_remove_active.svg");
    buttonConf.tooltip = "Удаление элементов графа";
    buttonConf.action = [this, buttonConf](QPushButton *pButton) -> void {
      emit requestModeClear();
      pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -1;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    return buttonConf;
}




}
