#include "plugininteractorwidget.hpp"
#include "ui_plugininteractorwidget.h"

#include <PluginModule/PluginMaster.h>
#include <PluginCoreInterface/PluginWidgets.h>
#include <PluginCoreInterface/Core.h>

#include <Components/Logger/Logger.h>

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
    auto& pMaster = Graph::PluginMaster::getInstance();
    auto plugin = pMaster.getPlugin(pluginName);
    auto pluginInteractor = plugin->getPluginCore()->getInteractor();
    if (!pluginInteractor) {
        LOG_WARNING("PluginInteractorWidget: No interactor found in a plugin", pluginName);
        return;
    }

    // Удаление на стороне плагина
    [[maybe_unused]] auto prevEditor = layout()->takeAt(0);
    layout()->addWidget(pluginInteractor);
}
