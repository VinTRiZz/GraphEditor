#ifndef ADDOBJECTMODE_H
#define ADDOBJECTMODE_H

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"
#include "graphmodebase.h"

namespace Graph {

class AddObjectMode : public GraphSubmodeBase {
public:
    AddObjectMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem* pTargetItem) override;
    void processMove(QGraphicsItem* pTargetItem,
                     const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pTargetItem) override;

private:
    ObjectViewItems::VertexObject* m_pendingVertex{nullptr};
    void setPendingVertex(ObjectViewItems::ItemBase* pItem);
};

}  // namespace Graph

#endif  // ADDOBJECTMODE_H
