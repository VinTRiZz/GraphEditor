#include "plugincore.hpp"

#include <Components/Logger/Logger.h>



namespace CrimeInspectionObjectName {
const QString PERSONVERTEX {"Person"};
}


CrimeInspectionPluginCore::CrimeInspectionPluginCore() :
    Graph::AbstractPluginCore(PLUGIN_NAME)
{
    registerObject(PluginObjectType::Vertex, CrimeInspectionObjectName::PERSONVERTEX);
}

CrimeInspectionPluginCore::~CrimeInspectionPluginCore()
{

}

Graph::PluginConfigurationWidget *CrimeInspectionPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginInteractionWidget *CrimeInspectionPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectInterface *CrimeInspectionPluginCore::createObject(const QString& name)
{
    if (name == CrimeInspectionObjectName::PERSONVERTEX) {
        return nullptr; // TODO: Create one
    }

    return nullptr;
}
