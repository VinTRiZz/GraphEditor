#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include "simplevertexitem.hpp"
#include "imagevertexitem.hpp"

namespace CommonPluginObjectName {
const QString SIMPLEVERTEX {"Simple vertex"};
const QString IMAGEVERTEX {"Image with name"};
}


CommonPluginCore::CommonPluginCore() :
    Graph::AbstractPluginCore()
{
    registerObject(PluginObjectType::Vertex, CommonPluginObjectName::SIMPLEVERTEX);
    registerObject(PluginObjectType::Vertex, CommonPluginObjectName::IMAGEVERTEX);

    LOG_DEBUG("Created instance of plugin core");
}

CommonPluginCore::~CommonPluginCore()
{
    LOG_DEBUG("Deleted instance of plugin core");
}

Graph::PluginConfigurationWidget *CommonPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginItemPropertyWidget *CommonPluginCore::getPropertyEditor(Graph::PluginObjectItnterface *pTarget)
{
    return nullptr;
}

Graph::PluginInteractionWidget *CommonPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectItnterface *CommonPluginCore::createObject(const QString& name)
{
    if (name == CommonPluginObjectName::SIMPLEVERTEX) {
        return new Graph::SimpleVertexItem();
    }

    if (name == CommonPluginObjectName::IMAGEVERTEX) {
        return new Graph::ImageVertexItem();
    }

    return nullptr;
}
