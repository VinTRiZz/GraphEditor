#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include "simplevertexitem.hpp"
#include "imagevertexitem.hpp"

namespace CommonItemsPluginObjectName {
const QString SIMPLEVERTEX {"Simple vertex"};
const QString IMAGEVERTEX {"Image vertex"};
}


CommonItemsPluginCore::CommonItemsPluginCore() :
    Graph::AbstractPluginCore()
{
    registerObject(PluginObjectType::Vertex, CommonItemsPluginObjectName::SIMPLEVERTEX);
    registerObject(PluginObjectType::Vertex, CommonItemsPluginObjectName::IMAGEVERTEX);

    LOG_DEBUG("Created instance of plugin core");
}

CommonItemsPluginCore::~CommonItemsPluginCore()
{
    LOG_DEBUG("Deleted instance of plugin core");
}

Graph::PluginConfigurationWidget *CommonItemsPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginItemPropertyWidget *CommonItemsPluginCore::getPropertyEditor(Graph::PluginObjectItnterface *pTarget)
{
    return nullptr;
}

Graph::PluginInteractionWidget *CommonItemsPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectItnterface *CommonItemsPluginCore::createObject(const QString& name)
{
    if (name == CommonItemsPluginObjectName::SIMPLEVERTEX) {
        return new Graph::SimpleVertexItem();
    }

    if (name == CommonItemsPluginObjectName::IMAGEVERTEX) {
        return new Graph::ImageVertexItem();
    }

    return nullptr;
}
