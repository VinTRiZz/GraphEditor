#include "graphtoolbar.h"

#include <QFileDialog>

#include <QVariant>

#include <Filework/SaveMaster.h>

GraphToolbar::GraphToolbar(QWidget* parent) :
    ButtonToolbar::HeadWidget(parent)
{
    setButtonSize(QSize(35, 35));

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
    buttonInfo.icon = QIcon(":/common/images/icons/common/graph_cancel_changes.svg");
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
}

void GraphToolbar::setShowPropertiesEnabled(bool isSaveEnabled)
{
    setButtonEnabled(m_showPropertiesButtonIndex, isSaveEnabled);
}

void GraphToolbar::setSaveEnabled(bool isSaveEnabled)
{
    setButtonEnabled(m_saveButtonIndex, isSaveEnabled);
}

void GraphToolbar::setSaveAsEnabled(bool isSaveAsEnabled)
{
    setButtonEnabled(m_saveAsButtonIndex, isSaveAsEnabled);
}

void GraphToolbar::setLoadEnabled(bool isLoadEnabled)
{
    setButtonEnabled(m_loadButtonIndex, isLoadEnabled);
}
