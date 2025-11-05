#pragma once

#include <QWidget>

#include <GraphObject/Maintainer.h>

namespace Ui {
class SettingsManagementToolbar;
}

class SettingsEditDialog;

class SettingsManagementToolbar :
        public QWidget {
    Q_OBJECT

public:
    explicit SettingsManagementToolbar(QWidget *parent = nullptr);
    ~SettingsManagementToolbar();

private:
    Ui::SettingsManagementToolbar *ui;

    SettingsEditDialog* m_settingsEditDialog {nullptr};
};

