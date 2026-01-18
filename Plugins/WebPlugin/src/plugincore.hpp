#pragma once

#include <PluginCoreInterface/Export.h>

class WebPluginCore : public Graph::AbstractPluginCore
{
public:
    WebPluginCore();
    ~WebPluginCore();

    // AbstractPluginCore interface
    Graph::PluginConfigurationWidget *getConfigurationEditor() override;
    Graph::PluginInteractionWidget *getInteractor() override;

    Graph::PluginObjectInterface *createObject(const QString &name) override;
};


// Определение интерфейсной функции
Graph::AbstractPluginCore *createPluginCore() {
    return new WebPluginCore;
}
