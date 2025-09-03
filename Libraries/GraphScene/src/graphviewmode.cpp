#include "graphviewmode.h"

#include <Common/Logging.h>

#include "graphsceneview.h"

namespace Graph {

GraphViewMode::GraphViewMode(QWidget* parent) : GraphModeBase(parent) {}

void GraphViewMode::init() {}

GraphViewMode::SearchProxy GraphViewMode::toSearchCache(
    const GVertex& vert) const {
    return {};
}

GraphViewMode::SearchProxy GraphViewMode::toSearchCache(
    const GConnection& con) const {
    return {};
}

void GraphViewMode::registerGraph() {
    connect(
        getScene()->getGraphMaintaner()->getExtendedObject(),
        &GraphExtendedObject::vertexAdded, this,
        [this](ObjectViewConstants::objectId_t addedId) {
            auto addedVertex =
                getScene()->getGraphMaintaner()->getExtendedObject()->getVertex(
                    addedId);
            if (!addedVertex.has_value()) {
                LOG_WARNING_SYNC(
                    "Error occured: getVertex() returned no value, but vertex "
                    "added");
                return;
            }
            objectsSearchCache.emplace(addedId,
                                       toSearchCache(addedVertex.value()));
        });
}

}  // namespace Graph
