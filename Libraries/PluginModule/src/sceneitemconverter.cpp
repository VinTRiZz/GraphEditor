#include "sceneitemconverter.hpp"

#include <Components/Logger/Logger.h>

#include <GraphItems/GraphItemsConstants.h>
#include <GraphItems/VertexItem.h>
#include <GraphItems/VertexConnectionItem.h>

#include <PluginModule/PluginMaster.h>
#include <PluginCoreInterface/Core.h>

namespace Graph {

std::list<QGraphicsItem*> SceneItemConverter::fromGraph(
    const GraphObject& graph) {
    std::list<QGraphicsItem*> res;

    auto& pluginMaster = PluginMaster::getInstance();

    auto& vertices = graph.getAllVertices();
    for (auto& vert : vertices) {
        auto pluginInfo = vert.getCommonData();

        auto pPluginCore = pluginMaster.getPlugin(pluginInfo[GObjectValueName::COMMON_PLUGIN_NAME].toString())->getPluginCore();
        auto* pVert = pPluginCore->createObject(pluginInfo[GObjectValueName::COMMON_PLUGIN_OBJECTNAME].toString());

        static_cast<VertexItem*>(pVert)->fromGObject(vert);
    }

    // Тут все объекты -- вершины 100%
    for (auto& vert : vertices) { // Соединяем вершины
        for (auto* pItemFrom : res) {
            auto pVertFrom = static_cast<Graph::VertexItem*>(pItemFrom);

            for (auto id : vert.getConnections()) {
                for (auto* pItemTo : res) {
                    auto pVertTo = static_cast<Graph::VertexItem*>(pItemTo);
                    if (pVertTo->getItemId() != id.value()) {
                        continue;
                    }

                    // TODO: Создать соединение от pVertFrom до pVertTo
                }
            }
        }
    }

    return res;
}

void SceneItemConverter::toGraph(const std::list<ObjectItems::BasicItem *> &items, const GraphObjectManagerPtr &pGraph)
{
    auto pGraphObj = pGraph->getObject();
    pGraphObj->clearGraphData();

    for (auto* pItem : items) {
        if (auto pVertex = dynamic_cast<VertexItem*>(pItem); nullptr != pVertex) {
            pGraphObj->addVertex(pVertex->toGObject());
        }
    }
}

}  // namespace Graph
