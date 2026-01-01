#include "plugincore.hpp"

#include <Components/Logger/Logger.h>



namespace WebPluginObjectName {
const QString LINKVERTEX {"Link"};
}


WebPluginCore::WebPluginCore() :
    Graph::AbstractPluginCore(PLUGIN_NAME)
{
    registerObject(PluginObjectType::Vertex, WebPluginObjectName::LINKVERTEX);
}

WebPluginCore::~WebPluginCore()
{

}

Graph::PluginConfigurationWidget *WebPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginItemPropertyWidget *WebPluginCore::getPropertyEditor(Graph::PluginObjectInterface *pTarget)
{
    return nullptr;
}

Graph::PluginInteractionWidget *WebPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectInterface *WebPluginCore::createObject(const QString& name)
{
    if (name == WebPluginObjectName::LINKVERTEX) {
        return nullptr; // TODO: Create
    }

    return nullptr;
}
