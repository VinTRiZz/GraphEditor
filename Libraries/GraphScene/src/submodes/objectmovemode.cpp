#include "objectmovemode.h"

namespace Graph {

void ObjectMoveMode::clearMode()
{

}

void ObjectMoveMode::processPress(QGraphicsItem *pItem)
{

}

void ObjectMoveMode::processMove(QGraphicsItem *pItem, const QPointF &currentPos)
{

}

void ObjectMoveMode::processRelease(QGraphicsItem *pItem)
{

}

ButtonMatrix::ButtonConfig ObjectMoveMode::getStarterButton()
{
    ButtonMatrix::ButtonConfig buttonConf;

    buttonConf = {};
    buttonConf.icon = QIcon(":/common/images/icons/editmode/mode_edit_move.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_move_active.svg");
    buttonConf.tooltip = "Перемещение вершин графа";
    buttonConf.action = [this, buttonConf](QPushButton *pButton) -> void {
      clearMode();
      pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -4;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;

    return buttonConf;
}

}
