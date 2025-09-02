#ifndef REMOVEOBJECTMODE_H
#define REMOVEOBJECTMODE_H

#include "graphmodebase.h"

namespace Graph
{

class RemoveObjectMode : public GraphSubmodeBase
{
public:
    using GraphSubmodeBase::GraphSubmodeBase;

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem *pTargetItem) override;
    void processMove(QGraphicsItem *pTargetItem, const QPointF &currentPos) override;
    void processRelease(QGraphicsItem *pTargetItem) override;
    ButtonMatrix::ButtonConfig getStarterButton() override;
};

}

#endif // REMOVEOBJECTMODE_H
