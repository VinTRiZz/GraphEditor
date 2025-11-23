#include "secondarytoolbar.hpp"
#include "ui_secondarytoolbar.h"

#include "settingseditdialog.hpp"

SecondaryToolbar::SecondaryToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SecondaryToolbar)
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

SecondaryToolbar::~SecondaryToolbar()
{
    delete ui;
}
