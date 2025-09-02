#ifndef ADDOBJECTMODE_H
#define ADDOBJECTMODE_H

#include "graphmodebase.h"

#include "Items/vertexobjectitem.h"
#include "Items/connectionlineitem.h"

namespace Graph {

class AddObjectMode : public GraphSubmodeBase
{
public:
    using GraphSubmodeBase::GraphSubmodeBase;

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem *pTargetItem) override;
    void processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos) override;
    void processRelease(QGraphicsItem *pTargetItem) override;
    ButtonMatrix::ButtonConfig getStarterButton() override;

private:
    ObjectViewItems::VertexObject* m_pendingVertex{nullptr};
    void setPendingVertex(ObjectViewItems::ItemBase* pItem);
};

}

#endif // ADDOBJECTMODE_H
