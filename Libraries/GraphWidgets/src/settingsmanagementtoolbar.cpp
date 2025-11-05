#include "settingsmanagementtoolbar.hpp"
#include "ui_settingsmanagementtoolbar.h"

#include "settingseditdialog.hpp"

SettingsManagementToolbar::SettingsManagementToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsManagementToolbar)
{
    ui->setupUi(this);

    connect(ui->openSettins_toolButton, &QToolButton::clicked,
            this, [this](){
        if (!m_settingsEditDialog) {
            m_settingsEditDialog = new SettingsEditDialog(this);
        }
        m_settingsEditDialog->show();
    });
}

SettingsManagementToolbar::~SettingsManagementToolbar()
{
    delete ui;
}
