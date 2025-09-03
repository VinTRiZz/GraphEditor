#ifndef ADDLINEMODE_H
#define ADDLINEMODE_H

#include "graphmodebase.h"

#include "Items/vertexobjectitem.h"
#include "Items/connectionlineitem.h"

namespace Graph
{

class AddLineMode : public GraphSubmodeBase
{
public:
    AddLineMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem *pTargetItem) override;
    void processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos) override;
    void processRelease(QGraphicsItem *pTargetItem) override;

private:
    ObjectViewItems::VertexConnectionLine* m_pendingConnectionLine{nullptr};
};

}

#endif // ADDLINEMODE_H
