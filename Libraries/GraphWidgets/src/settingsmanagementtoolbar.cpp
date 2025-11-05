#include "settingsmanagementtoolbar.hpp"
#include "ui_settingsmanagementtoolbar.h"

SettingsManagementToolbar::SettingsManagementToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsManagementToolbar)
{
    ui->setupUi(this);
}

SettingsManagementToolbar::~SettingsManagementToolbar()
{
    delete ui;
}
