#ifndef OBJECTMOVEMODE_H
#define OBJECTMOVEMODE_H

#include <GraphItems/VertexObjectItem.h>
#include <GraphItems/VertexConnectionItem.h>
#include "graphmodebase.h"

namespace Graph {

class ObjectMoveMode : public GraphSubmodeBase {
public:
    ObjectMoveMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem* pTargetItem) override;
    void processMove(QGraphicsItem* pTargetItem,
                     const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pTargetItem) override;

private:
    Graph::VertexObject* m_movingVertex{nullptr};
    Graph::VertexConnectionLine* m_movingConnectionLine{nullptr};
};

}  // namespace Graph

#endif  // OBJECTMOVEMODE_H
