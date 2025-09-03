#ifndef ADDLINEMODE_H
#define ADDLINEMODE_H

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"
#include "graphmodebase.h"

namespace Graph {

class AddLineMode : public GraphSubmodeBase {
public:
    AddLineMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem* pTargetItem) override;
    void processMove(QGraphicsItem* pTargetItem,
                     const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pTargetItem) override;

private:
    ObjectViewItems::VertexConnectionLine* m_pendingConnectionLine{nullptr};
};

}  // namespace Graph

#endif  // ADDLINEMODE_H
