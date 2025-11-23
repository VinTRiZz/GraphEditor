#include "plugininteractorwidget.hpp"
#include "ui_plugininteractorwidget.h"

PluginInteractorWidget::PluginInteractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginInteractorWidget)
{
    ui->setupUi(this);
}

PluginInteractorWidget::~PluginInteractorWidget()
{
    delete ui;
}

void PluginInteractorWidget::setCurrentPlugin(const QString &pluginName)
{
    // TODO: Create interactor
}
