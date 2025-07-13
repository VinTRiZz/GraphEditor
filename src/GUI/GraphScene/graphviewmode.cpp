#include "graphviewmode.h"

#include "graphscenebase.h"

#include "logging.h"

namespace Graph
{

GraphViewMode::GraphViewMode(QWidget *parent) :
    GraphModeBase(parent)
{
    
}

void GraphViewMode::init()
{

}

void GraphViewMode::start()
{
    // TODO: Setup button matrix

    setStarted();
}

void GraphViewMode::stop()
{
    // TODO: Reset button matrix

    setStopped();
}

void GraphViewMode::processPress(QGraphicsItem *pItem)
{
    
}

void GraphViewMode::processMove(QGraphicsItem *pItem, const QPointF &currentPos)
{
    
}

void GraphViewMode::processRelease(QGraphicsItem *pItem)
{
    
}

GraphViewMode::SearchProxy GraphViewMode::toSearchCache(const GVertex &vert) const
{
    return {};
}

GraphViewMode::SearchProxy GraphViewMode::toSearchCache(const GConnection &con) const
{
    return {};
}

void GraphViewMode::registerGraph()
{
    connect(getScene()->getCurrentGraph(), &GraphExtendedObject::vertexAdded,
            this, [this](ObjectSceneConstants::objectId_t addedId) {
        auto addedVertex = getScene()->getCurrentGraph()->getVertex(addedId);
        if (!addedVertex.has_value()) {
            LOG_WARNING_SYNC("Error occured: getVertex() returned no value, but vertex added");
            return;
        }
        objectsSearchCache.emplace(addedId, toSearchCache(addedVertex.value()));
    });
}

}
