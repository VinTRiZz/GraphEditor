#include "graphfilestoolbar.h"

#include <Filework/SaveMaster.h>

#include <QFileDialog>
#include <QVariant>

GraphFilesToolbar::GraphFilesToolbar(QWidget* parent)
    : ButtonToolbar::HeadWidget(parent) {
    setButtonSize(QSize(35, 35));
    setFixedHeight(37);

    ButtonToolbar::ButtonConfig buttonInfo;
    buttonInfo.buttonPos = 0;

    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_new.svg");
    buttonInfo.tooltip = "Создать граф";
    buttonInfo.action = [this](QPushButton*) {
        emit createGraph();
    };
    addButton(buttonInfo);

    buttonInfo.buttonPos++;
    m_loadButtonIndex = buttonInfo.buttonPos;
    buttonInfo.icon =
        QIcon(":/common/images/icons/common/graph_cancel_changes.svg");
    buttonInfo.tooltip = "Отменить изменения";
    buttonInfo.action = [this](QPushButton*) {
        emit loadGraph({});
    };
    buttonInfo.isEnabled = false;
    addButton(buttonInfo);
    buttonInfo.isEnabled = true;

    buttonInfo.buttonPos++;
    m_saveButtonIndex = buttonInfo.buttonPos;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_save.svg");
    buttonInfo.tooltip = "Сохранить";
    buttonInfo.action = [this](QPushButton*) mutable {
        emit saveGraph({});
        setLoadEnabled(true);
    };
    buttonInfo.isEnabled = false;
    addButton(buttonInfo);
    buttonInfo.isEnabled = true;

    buttonInfo.buttonPos++;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_open.svg");
    buttonInfo.tooltip = "Открыть файл графа";
    buttonInfo.action = [this](QPushButton* pButton) {
        auto graphPath = SaveMaster::getLoadPath();
        if (graphPath.isEmpty()) {
            return;
        }
        loadGraph(graphPath);
    };
    addButton(buttonInfo);

    buttonInfo.buttonPos++;
    m_saveAsButtonIndex = buttonInfo.buttonPos;
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_save_as.svg");
    buttonInfo.tooltip = "Сохранить как...";
    buttonInfo.action = [this](QPushButton* pButton) {
        auto graphPath = SaveMaster::getSavePath();
        if (graphPath.isEmpty()) {
            return;
        }
        emit saveGraph(graphPath);
    };
    addButton(buttonInfo);

    buttonInfo.buttonPos++;
    m_showPropertiesButtonIndex = buttonInfo.buttonPos;
    buttonInfo.icon = QIcon(":/common/images/icons/common/edit_properties.svg");
    buttonInfo.tooltip = "Показать свойства графа";
    buttonInfo.action = [this](QPushButton* pSender) {
        const auto propertyIsHiddenName{"isPropertiesHidden"};

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
}

void GraphFilesToolbar::setShowPropertiesEnabled(bool isPropOpenEnabled) {
    setButtonEnabled(m_showPropertiesButtonIndex, isPropOpenEnabled);
}

void GraphFilesToolbar::setSaveEnabled(bool isSaveEnabled) {
    setButtonEnabled(m_saveButtonIndex, isSaveEnabled);
}

void GraphFilesToolbar::setSaveAsEnabled(bool isSaveAsEnabled) {
    setButtonEnabled(m_saveAsButtonIndex, isSaveAsEnabled);
}

void GraphFilesToolbar::setLoadEnabled(bool isLoadEnabled) {
    setButtonEnabled(m_loadButtonIndex, isLoadEnabled);
}
