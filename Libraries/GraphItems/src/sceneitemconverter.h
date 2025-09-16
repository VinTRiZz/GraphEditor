#ifndef SCENEITEMCONVERTER_H
#define SCENEITEMCONVERTER_H

#include <GraphObject/Maintainer.h>
#include <ObjectItems/ItemBase.h>

#include "connectionlineitem.h"
#include "vertexobjectitem.h"

namespace Graph {

class SceneItemConverter {
public:
    static ObjectViewItems::VertexObject* fromVertex(
        const Graph::GVertex& vert);
    static ObjectViewItems::VertexConnectionLine* fromConnection(
        const Graph::GConnection& con);

    static std::list<ObjectViewItems::ItemBase*> fromGraph(
        const Graph::GraphObject& graph);

    static std::list<ObjectViewItems::ItemBase*> fromMaintainer(
        const Graph::PMaintainer& pMaintainer);

    static Graph::GVertex toVertex(const ObjectViewItems::ItemBase* item);
    static Graph::GConnection toConnection(
        const ObjectViewItems::ItemBase* item);

    static Graph::GraphObject toGraph(
        const std::list<ObjectViewItems::ItemBase*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<ObjectViewItems::ItemBase*>& items);
};

}  // namespace Graph

#endif  // SCENEITEMCONVERTER_H
