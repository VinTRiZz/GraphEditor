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

public slots:
    void setCurrentPlugin(const QString& pluginName);

private:
    Ui::PluginInteractorWidget *ui;
};

