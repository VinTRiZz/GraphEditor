#pragma once

#include <QWidget>

namespace Ui {
class PluginInteractorWidget;
}

class PluginInteractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PluginInteractorWidget(QWidget *parent = nullptr);
    ~PluginInteractorWidget();

private:
    Ui::PluginInteractorWidget *ui;
};

