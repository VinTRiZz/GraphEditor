#include "sceneitemconverter.h"

#include <Common/Logging.h>

namespace Graph {

ObjectViewItems::VertexObject* SceneItemConverter::fromVertex(
    const GVertex& vert) {
    auto pVertexItem = new ObjectViewItems::VertexObject;

    pVertexItem->setObjectId(vert.id);
    pVertexItem->setPos(vert.posX, vert.posY);

    pVertexItem->setShortName(vert.shortName);
    pVertexItem->setName(vert.name);
    pVertexItem->setDescription(vert.description);

    pVertexItem->setMainColor(vert.borderColor);
    pVertexItem->setSecondColor(vert.backgroundColor);

    auto& sceneConfig =
        ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    pVertexItem->setZValue(sceneConfig.vertexLayer);

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);
    pVertexItem->setRect(vertexRect);

    if (!vert.image.isNull()) {
        pVertexItem->setImage(vert.image);
    }

    return pVertexItem;
}

ObjectViewItems::VertexConnectionLine* SceneItemConverter::fromConnection(
    const GConnection& con) {
    auto pConnection = new ObjectViewItems::VertexConnectionLine;

    pConnection->setName(con.name);
    pConnection->setMainColor(con.lineColor);
    pConnection->setWeight(con.connectionWeight);

    pConnection->setZValue(
        ObjectViewConstants::ObjectSceneConfiguration::getInstance()
            .connectionLineLayer);
    return pConnection;
}

std::list<ObjectViewItems::ItemBase*> SceneItemConverter::fromGraph(
    const GraphObject& graph) {
    std::list<ObjectViewItems::ItemBase*> res;

    auto vertices = graph.getAllVertices();
    std::unordered_map<GraphCommon::graphId_t, ObjectViewItems::VertexObject*>
        vertexObjects;

    for (auto& vert : vertices) {
        res.push_back(fromVertex(vert));
        vertexObjects[vert.id] =
            static_cast<ObjectViewItems::VertexObject*>(res.back());
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

        auto pConnection = fromConnection(con);
        pConFrom->second->subscribeAsConnectionFrom(pConnection);
        pConTo->second->subscribeAsConnectionTo(pConnection);
        res.push_back(pConnection);
    }

    return res;
}

std::list<ObjectViewItems::ItemBase*> SceneItemConverter::fromMaintainer(
    const PMaintainer& pMaintainer) {
    const auto& rGraph = pMaintainer->getObject();
    std::list<ObjectViewItems::ItemBase*> res = fromGraph(rGraph);

    // Пространство для воображения

    return res;
}

GVertex SceneItemConverter::toVertex(const ObjectViewItems::ItemBase* item) {
    auto vertCasted = static_cast<const ObjectViewItems::VertexObject*>(item);

    GVertex graphVertex;
    graphVertex.id = vertCasted->getObjectId();
    graphVertex.posX = vertCasted->x();
    graphVertex.posY = vertCasted->y();

    graphVertex.shortName = vertCasted->getShortName();
    graphVertex.name = vertCasted->getName();
    graphVertex.description = vertCasted->getDescription();

    graphVertex.borderColor = vertCasted->getMainColor();
    graphVertex.backgroundColor = vertCasted->getSecondColor();

    graphVertex.image = vertCasted->getImage();

    return graphVertex;
}

GConnection SceneItemConverter::toConnection(
    const ObjectViewItems::ItemBase* item) {
    auto conCasted =
        static_cast<const ObjectViewItems::VertexConnectionLine*>(item);

    // Игнорируем невалидные соединения (например, которые в состоянии
    // редактирования)
    if (conCasted->getVertexFrom() == nullptr ||
        conCasted->getVertexTo() == nullptr) {
        LOG_WARNING("Skipped invalid connection:", conCasted->getName());
        return {};
    }

    GConnection graphConnection;

    graphConnection.idFrom = conCasted->getVertexFrom()->getObjectId();
    graphConnection.idTo = conCasted->getVertexTo()->getObjectId();

    graphConnection.name = conCasted->getName();
    graphConnection.lineColor = conCasted->getMainColor();
    graphConnection.connectionWeight = conCasted->getWeight();

    return graphConnection;
}

GraphObject SceneItemConverter::toGraph(
    const std::list<ObjectViewItems::ItemBase*>& items) {
    GraphObject res;

    LOG_INFO("Loading vertices from scene...");
    for (auto vert : items) {
        if (vert->getType() != ObjectViewConstants::OBJECTTYPE_VERTEX) {
            continue;
        }
        res.addVertex(toVertex(vert));
    }
    LOG_OK("Loaded", res.getVerticesCount(), "vertices from scene");

    LOG_INFO("Loading connections from scene...");
    for (auto con : items) {
        if (con->getType() !=
            ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION) {
            continue;
        }
        res.addConnection(toConnection(con));
    }
    LOG_OK("Loaded", res.getConnectionsCount(), "connections from scene");

    return res;
}

void SceneItemConverter::toMaintainer(
    PMaintainer& pMaintainer,
    const std::list<ObjectViewItems::ItemBase*>& items) {
    pMaintainer->getObject() = toGraph(items);

    // Пространство для воображения
}

}  // namespace Graph
