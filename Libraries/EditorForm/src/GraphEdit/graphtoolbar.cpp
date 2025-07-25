#include "graphtoolbar.h"

#include <QFileDialog>

#include <QVariant>

GraphToolbar::GraphToolbar(QWidget* parent) :
    ButtonToolbar::HeadWidget(parent)
{
    init();
}

void GraphToolbar::init()
{
    setButtonSize(QSize(35, 35));

    ButtonToolbar::ButtonConfig buttonInfo;

    buttonInfo.icon = QIcon(":/common/images/icons/common/edit_properties.svg");
    buttonInfo.tooltip = "Показать свойства графа";
    buttonInfo.action = [this](QPushButton* pSender) {
        const auto propertyIsHiddenName {"isPropertiesHidden"};

        if (pSender->property(propertyIsHiddenName).toBool()) {
            pSender->setToolTip("Показать свойства графа");
            emit hideProperties();
            pSender->setProperty(propertyIsHiddenName, false);
            return;
        }
        pSender->setToolTip("Скрыть свойства графа");
        emit showProperties();
        pSender->setProperty(propertyIsHiddenName, true);
    };
    addButton(buttonInfo);


    buttonInfo.buttonPos++;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_cancel_changes.svg");
    buttonInfo.tooltip = "Отменить изменения";
    buttonInfo.action = [this](QPushButton*) {
        emit loadGraph({});
    };
    buttonInfo.isEnabled = false;
    addButton(buttonInfo);
    buttonInfo.isEnabled = true;


    auto cancelChangesPos = buttonInfo.buttonPos;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_save.svg");
    buttonInfo.tooltip = "Сохранить";
    buttonInfo.buttonPos++;
    buttonInfo.action = [this, cancelChangesPos](QPushButton*) mutable {
        emit saveGraph({});
        setButtonEnabled(cancelChangesPos, true);
    };
    buttonInfo.isEnabled = false;
    addButton(buttonInfo);
    buttonInfo.isEnabled = true;


    auto saveChangesChangesPos = buttonInfo.buttonPos;
    buttonInfo.buttonPos++;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_open.svg");
    buttonInfo.tooltip = "Открыть файл графа";
    buttonInfo.action = [this, saveChangesChangesPos](QPushButton* pButton) {
        auto graphPath = QFileDialog::getOpenFileName(this, "Файл сохранённого графа", QDir::homePath(), "Файл графа (*.gse)");
        if (graphPath.isEmpty()) {
            return;
        }
        loadGraph(graphPath);
        setButtonEnabled(saveChangesChangesPos, true);
    };
    addButton(buttonInfo);


    buttonInfo.buttonPos++;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_save_as.svg");
    buttonInfo.tooltip = "Сохранить как...";
    buttonInfo.action = [this, saveChangesChangesPos](QPushButton* pButton) {
        auto graphPath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");
        if (graphPath.isEmpty()) {
            return;
        }
        emit saveGraph(graphPath);
        setButtonEnabled(saveChangesChangesPos, true);
    };
    addButton(buttonInfo);

//    buttonInfo.buttonPos++;
//    buttonInfo.icon = QIcon(":/common/images/icons/common/mode_change.svg");
//    buttonInfo.tooltip = "Сменить режим работы";
//    buttonInfo.action = [this](QPushButton*) {
//        emit changeMode();
//    };

//    addButton(buttonInfo);
}
