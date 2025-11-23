#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include <GraphItems/VertexItem.h>

namespace CrimeInspectionObjectName {
const QString PERSONVERTEX {"Person"};
}


CrimeInspectionPluginCore::CrimeInspectionPluginCore() :
    Graph::AbstractPluginCore()
{
    registerObject(PluginObjectType::Vertex, CrimeInspectionObjectName::PERSONVERTEX);

    LOG_DEBUG("Created instance of plugin core");
}

CrimeInspectionPluginCore::~CrimeInspectionPluginCore()
{
    LOG_DEBUG("Deleted instance of plugin core");
}

Graph::PluginConfigurationWidget *CrimeInspectionPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginItemPropertyWidget *CrimeInspectionPluginCore::getPropertyEditor(Graph::PluginObjectItnterface *pTarget)
{
    return nullptr;
}

Graph::PluginInteractionWidget *CrimeInspectionPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectItnterface *CrimeInspectionPluginCore::createObject(const QString& name)
{
    if (name == CrimeInspectionObjectName::PERSONVERTEX) {
        return nullptr; // TODO: Create one
    }

    return nullptr;
}
