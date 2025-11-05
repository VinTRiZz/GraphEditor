#include "settingseditdialog.hpp"
#include "ui_settingseditdialog.h"

SettingsEditDialog::SettingsEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsEditDialog)
{
    ui->setupUi(this);
}

SettingsEditDialog::~SettingsEditDialog()
{
    delete ui;
}
