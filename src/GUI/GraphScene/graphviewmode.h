#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include "graphmodebase.h"
#include "Graph/graphobject.h"

#include "GUI/ObjectScene/objectsceneconstants.h"
#include "GUI/CustomWidgets/buttonmatrix.h"

namespace Graph
{

class GraphViewMode : public GraphModeBase
{
    Q_OBJECT
public:
    GraphViewMode(QWidget* parent = nullptr);

    void init() override;

    void start() override;
    void stop() override;

    void processPress(QGraphicsItem* pItem) override;
    void processMove(QGraphicsItem* pItem, const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pItem) override;

private:
    struct SearchProxy
    {
        QStringView name;
        QStringView shortName;
        QStringView description;
    };
    std::map<ObjectSceneConstants::objectId_t, SearchProxy> objectsSearchCache;

    SearchProxy toSearchCache(const Graph::GVertex& vert) const;
    SearchProxy toSearchCache(const Graph::GConnection& con) const;

protected:
    void registerGraph();
};

}

#endif // GRAPHVIEWER_H
