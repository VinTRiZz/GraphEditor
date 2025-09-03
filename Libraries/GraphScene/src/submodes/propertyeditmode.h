#ifndef PROPERTYEDITMODE_H
#define PROPERTYEDITMODE_H

#include "graphmodebase.h"

namespace Graph {

class PropertyEditMode : public GraphSubmodeBase {
    Q_OBJECT
public:
    PropertyEditMode(GraphModeBase* pParentMode);

    // GraphSubmodeBase interface
    void clearMode() override;
    void processPress(QGraphicsItem* pTargetItem) override;
    void processMove(QGraphicsItem* pTargetItem,
                     const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pTargetItem) override;

signals:
    void openPropertyEditor(QGraphicsItem* pTargetItem);
    void closePropertyEditor();
};

}  // namespace Graph

#endif  // PROPERTYEDITMODE_H
