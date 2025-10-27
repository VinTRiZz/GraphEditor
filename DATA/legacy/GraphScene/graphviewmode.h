#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <Components/CustomQt/ButtonMatrix.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include "graphmodebase.h"

namespace Graph {

class GraphViewMode : public GraphModeBase {
    Q_OBJECT
public:
    GraphViewMode(QWidget* parent = nullptr);

    void init() override;

private:
    struct SearchProxy {
        QStringView name;
        QStringView shortName;
        QStringView description;
    };
    std::map<ObjectItems::objectId_t, SearchProxy> objectsSearchCache;

    SearchProxy toSearchCache(const Graph::GVertex& vert) const;
    SearchProxy toSearchCache(const Graph::GConnection& con) const;

protected:
    void registerGraph();
};

}  // namespace Graph

#endif  // GRAPHVIEWER_H
