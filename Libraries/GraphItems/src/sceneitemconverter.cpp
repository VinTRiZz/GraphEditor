#include "sceneitemconverter.hpp"

#include <Components/Logger/Logger.h>

#include "vertexobjectitem.hpp"
#include "connectionlineitem.hpp"

#include "constants.hpp"

namespace Graph {

std::list<QGraphicsItem*> SceneItemConverter::fromGraph(
    const GraphObject& graph) {
    std::list<QGraphicsItem*> res;

    auto vertices = graph.getAllVertices();
    std::unordered_map<GraphCommon::graphId_t, Graph::VertexObjectItem*>
        vertexObjects;

    for (auto& vert : vertices) {
        auto pVert = new VertexObjectItem;
        pVert->fromVertex(vert);
        res.push_back(pVert);
        vertexObjects[vert.id] =
            static_cast<Graph::VertexObjectItem*>(res.back());
    }

    QHash<GraphCommon::graphId_t, std::vector<GConnection>> connectionHash;
    for (auto& con : graph.getAllConnections()) {
        auto pConFrom = vertexObjects.find(con.idFrom);
        if (pConFrom == vertexObjects.end()) {
            throw std::runtime_error("Vertex from did not found!");
        }

        auto pConTo = vertexObjects.find(con.idTo);
        if (pConTo == vertexObjects.end()) {
            throw std::runtime_error("Target vertex did not found!");
        }

        auto pConnection = new VertexConnectionLine;
        pConnection->fromConnection(con);
        pConFrom->second->subscribeAsConnectionFrom(pConnection);
        pConTo->second->subscribeAsConnectionTo(pConnection);
        res.push_back(pConnection);
    }

    return res;
}

std::list<QGraphicsItem*> SceneItemConverter::fromMaintainer(
    const PMaintainer& pMaintainer) {
    const auto& rGraph = pMaintainer->getObject();
    std::list<QGraphicsItem*> res = fromGraph(rGraph);

    // Пространство для воображения

    return res;
}

GraphObject SceneItemConverter::toGraph(
    const std::list<ObjectItems::BasicItem*>& items) {
    GraphObject res;

    LOG_INFO("Loading vertices from scene...");
    for (auto vert : items) {
        if (vert->getObjectType() != OBJECTTYPE_VERTEX) {
            continue;
        }
        res.addVertex(static_cast<Graph::VertexItem*>(vert)->toVertex());
    }
    LOG_OK("Loaded", res.getVerticesCount(), "vertices from scene");

    LOG_INFO("Loading connections from scene...");
    for (auto con : items) {
        if (con->getObjectType() !=
            OBJECTTYPE_CONNECTION) {
            continue;
        }
        res.addConnection(static_cast<Graph::VertexConnectionLine*>(con)->toConnection());
    }
    LOG_OK("Loaded", res.getConnectionsCount(), "connections from scene");

    return res;
}

GraphObject SceneItemConverter::toGraph(const std::list<QGraphicsItem *> &items)
{
    std::list<ObjectItems::BasicItem*> itemsCasted;
    for (auto* pItem : items) {
        auto pCastedItem = dynamic_cast<ObjectItems::BasicItem*>(pItem);
        if (pCastedItem) {
            itemsCasted.push_back(pCastedItem);
        }
    }
    return toGraph(itemsCasted);
}

void SceneItemConverter::toMaintainer(
    PMaintainer& pMaintainer,
    const std::list<ObjectItems::BasicItem*>& items) {
    pMaintainer->getObject() = toGraph(items);

    // Пространство для воображения
}

void SceneItemConverter::toMaintainer(PMaintainer &pMaintainer, const std::list<QGraphicsItem *> &items)
{
    pMaintainer->getObject() = toGraph(items);

    // Пространство для воображения
}

}  // namespace Graph
