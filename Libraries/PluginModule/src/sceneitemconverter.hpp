#pragma once

#include <GraphObject/GraphObject.h>
#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

class SceneItemConverter {
public:
    static std::list<QGraphicsItem*> fromGraph(
        const Graph::GraphObject& graph);

    static void toGraph(
        const std::list<ObjectItems::BasicItem*>& items,
        const Graph::GraphObjectManagerPtr& pGraph);
};

}  // namespace Graph
