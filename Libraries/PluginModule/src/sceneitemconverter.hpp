#pragma once

#include <GraphObject/Maintainer.h>
#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

class SceneItemConverter {
public:
    static std::list<QGraphicsItem*> fromGraph(
        const Graph::GraphObject& graph);

    static std::list<QGraphicsItem*> fromMaintainer(
        const Graph::PMaintainer& pMaintainer);

    static Graph::GraphObject toGraph(
        const std::list<ObjectItems::BasicItem*>& items);

    // Не рекомендуется к использованию (динамик касты)
    static Graph::GraphObject toGraph(
        const std::list<QGraphicsItem*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<ObjectItems::BasicItem*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<QGraphicsItem*>& items);
};

}  // namespace Graph
