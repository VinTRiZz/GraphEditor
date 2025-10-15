#ifndef SCENEITEMCONVERTER_H
#define SCENEITEMCONVERTER_H

#include <GraphObject/Maintainer.h>
#include <Components/CustomQt/ObjectScene/ItemBase.h>

namespace Graph {

class SceneItemConverter {
public:
    static std::list<ObjectViewItems::ItemBase*> fromGraph(
        const Graph::GraphObject& graph);

    static std::list<ObjectViewItems::ItemBase*> fromMaintainer(
        const Graph::PMaintainer& pMaintainer);

    static Graph::GraphObject toGraph(
        const std::list<ObjectViewItems::ItemBase*>& items);

    static void toMaintainer(
        Graph::PMaintainer& pMaintainer,
        const std::list<ObjectViewItems::ItemBase*>& items);
};

}  // namespace Graph

#endif  // SCENEITEMCONVERTER_H
