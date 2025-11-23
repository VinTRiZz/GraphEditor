#pragma once

#include <QWidget>

#include <GraphObject/Maintainer.h>

namespace Ui {
class SecondaryToolbar;
}

class SettingsEditDialog;

class SecondaryToolbar :
        public QWidget {
    Q_OBJECT

public:
    explicit SecondaryToolbar(QWidget *parent = nullptr);
    ~SecondaryToolbar();

private:
    Ui::SecondaryToolbar *ui;

    SettingsEditDialog* m_settingsEditDialog {nullptr};
};

