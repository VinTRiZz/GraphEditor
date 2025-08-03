#include "graphoptionstoolbar.h"

#include <Filework/SaveMaster.h>

#include <QFileDialog>
#include <QVariant>

GraphOptionsToolbar::GraphOptionsToolbar(QWidget* parent)
    : ButtonToolbar::HeadWidget(parent) {
    setButtonSize(QSize(35, 35));
    setFixedWidth(37);
    setVertical();

    ButtonToolbar::ButtonConfig buttonInfo;
    buttonInfo.buttonPos = 0;

    buttonInfo.icon = QIcon(":/common/images/icons/common/settings.svg");
    buttonInfo.tooltip = "Настройки приложения";
    buttonInfo.action = [this](QPushButton*) {
        emit openSettings();
    };
    addButton(buttonInfo);
}
