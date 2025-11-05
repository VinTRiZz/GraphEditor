#pragma once

#include <QDialog>

namespace Ui {
class SettingsEditDialog;
}

class SettingsEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsEditDialog(QWidget *parent = nullptr);
    ~SettingsEditDialog();

private:
    Ui::SettingsEditDialog *ui;
};

