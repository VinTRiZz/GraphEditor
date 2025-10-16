#ifndef SCENEITEMCONVERTER_H
#define SCENEITEMCONVERTER_H

#include <GraphObject/Maintainer.h>
#include "graphsceneitem.hpp"

namespace Graph {

class SceneItemConverter {
public:
    static std::list<GraphSceneItem*> fromGraph(
        const Graph::GraphObject& graph);

    static std::list<GraphSceneItem*> fromMaintainer(
        const Graph::PMaintainer& pMaintainer);

    static Graph::GraphObject toGraph(
        const std::list<ObjectViewItems::ItemBase*>& items);

    static Graph::GraphObject toGraph(
        const std::list<GraphSceneItem*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<ObjectViewItems::ItemBase*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<GraphSceneItem*>& items);
};

}  // namespace Graph

#endif  // SCENEITEMCONVERTER_H
