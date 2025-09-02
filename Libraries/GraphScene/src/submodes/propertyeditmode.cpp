#include "propertyeditmode.h"

#include "graphsceneview.h"

namespace Graph
{

void PropertyEditMode::clearMode()
{

}

void PropertyEditMode::processPress(QGraphicsItem *pTargetItem)
{

}

void PropertyEditMode::processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos)
{

}

void PropertyEditMode::processRelease(QGraphicsItem *pTargetItem)
{
    auto pItem = dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem);
    if (pItem == nullptr) {
      return;
    }

    // Игнорируем это, пусть пользователь редактирует свойства
    if (pItem->getType() == ObjectViewConstants::OBJECTTYPE_PROPERTY_EDITOR) {
      return;
    }

    emit openPropertyEditor(pItem);
}

ButtonMatrix::ButtonConfig PropertyEditMode::getStarterButton()
{
    ButtonMatrix::ButtonConfig buttonConf;
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_properties.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_properties_active.svg");
    buttonConf.tooltip = "Изменение свойств вершины";
    buttonConf.action = [this, buttonConf](QPushButton *pButton) -> void {
      emit requestModeClear();
      pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = 0;
    buttonConf.positionY = 1;
    buttonConf.isEnabled = true;
    return buttonConf;
}

}
