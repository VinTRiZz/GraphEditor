#ifndef OBJECTMOVEMODE_H
#define OBJECTMOVEMODE_H

#include "graphmodebase.h"

#include "Items/vertexobjectitem.h"
#include "Items/connectionlineitem.h"

namespace Graph {

class ObjectMoveMode : public GraphSubmodeBase
{
public:
    ObjectMoveMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem *pTargetItem) override;
    void processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos) override;
    void processRelease(QGraphicsItem *pTargetItem) override;

private:
    ObjectViewItems::VertexObject* m_movingVertex{nullptr};
    ObjectViewItems::VertexConnectionLine* m_movingConnectionLine{nullptr};
};

}

#endif // OBJECTMOVEMODE_H
