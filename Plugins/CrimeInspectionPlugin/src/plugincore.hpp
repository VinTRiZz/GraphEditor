#pragma once

#include <PluginCoreInterface/Core.h>

class CrimeInspectionPluginCore : public Graph::AbstractPluginCore
{
public:
    CrimeInspectionPluginCore();
    ~CrimeInspectionPluginCore();

    // AbstractPluginCore interface
    Graph::PluginConfigurationWidget *getConfigurationEditor() override;
    Graph::PluginItemPropertyWidget *getPropertyEditor(Graph::PluginObjectItnterface *pTarget) override;
    Graph::PluginInteractionWidget *getInteractor() override;

    Graph::PluginObjectItnterface *createObject(const QString &name) override;
};


// Определение интерфейсной функции
Graph::AbstractPluginCore *createPluginCore() {
    return new CrimeInspectionPluginCore;
}
