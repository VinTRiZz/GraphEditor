#pragma once

#include <QWidget>

namespace Ui {
class SettingsManagementToolbar;
}

class SettingsManagementToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsManagementToolbar(QWidget *parent = nullptr);
    ~SettingsManagementToolbar();

private:
    Ui::SettingsManagementToolbar *ui;
};

