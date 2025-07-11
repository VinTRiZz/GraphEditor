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
    bool isRunning() const override;
    void stop() override;

private:
    bool m_isModeStarted {false};

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
