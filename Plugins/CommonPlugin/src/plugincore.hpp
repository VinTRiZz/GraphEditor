#pragma once

#include <PluginCoreInterface/Export.h>

class CommonPluginCore : public Graph::AbstractPluginCore
{
public:
    CommonPluginCore();
    ~CommonPluginCore();

    // AbstractPluginCore interface
    Graph::PluginConfigurationWidget *getConfigurationEditor() override;
    Graph::PluginItemPropertyWidget *getPropertyEditor(Graph::PluginObjectInterface *pTarget) override;
    Graph::PluginInteractionWidget *getInteractor() override;

    Graph::PluginObjectInterface *createObject(const QString &name) override;
};


// Определение интерфейсной функции
Graph::AbstractPluginCore *createPluginCore() {
    return new CommonPluginCore;
}
