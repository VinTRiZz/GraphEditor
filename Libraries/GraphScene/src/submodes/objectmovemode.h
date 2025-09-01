#ifndef OBJECTMOVEMODE_H
#define OBJECTMOVEMODE_H

#include "graphmodebase.h"

#include "Items/vertexobjectitem.h"
#include "Items/connectionlineitem.h"

namespace Graph {

class ObjectMoveMode : public GraphSubmodeBase
{
public:
    void clearMode() override;

    // GraphSubmodeBase interface
    void processPress(QGraphicsItem *pItem) override;
    void processMove(QGraphicsItem *pItem, const QPointF &currentPos) override;
    void processRelease(QGraphicsItem *pItem) override;
    ButtonMatrix::ButtonConfig getStarterButton() override;

private:
    ObjectViewItems::VertexObject* m_movingVertex{nullptr};
    ObjectViewItems::VertexConnectionLine* m_movingConnectionLine{nullptr};
};

}

#endif // OBJECTMOVEMODE_H
