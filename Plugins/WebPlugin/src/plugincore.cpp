#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include <GraphItems/VertexItem.h>

namespace WebPluginObjectName {
const QString LINKVERTEX {"Link"};
}


WebPluginCore::WebPluginCore() :
    Graph::AbstractPluginCore()
{
    registerObject(PluginObjectType::Vertex, WebPluginObjectName::LINKVERTEX);

    LOG_DEBUG("Created instance of plugin core");
}

WebPluginCore::~WebPluginCore()
{
    LOG_DEBUG("Deleted instance of plugin core");
}

Graph::PluginConfigurationWidget *WebPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginItemPropertyWidget *WebPluginCore::getPropertyEditor(Graph::PluginObjectItnterface *pTarget)
{
    return nullptr;
}

Graph::PluginInteractionWidget *WebPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectItnterface *WebPluginCore::createObject(const QString& name)
{
    if (name == WebPluginObjectName::LINKVERTEX) {
        return nullptr; // TODO: Create
    }

    return nullptr;
}
