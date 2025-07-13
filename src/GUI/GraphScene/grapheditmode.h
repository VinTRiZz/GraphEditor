#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "graphmodebase.h"

namespace Graph
{

class GraphEditMode : public GraphModeBase
{
    Q_OBJECT
public:
    GraphEditMode(QWidget* parent = nullptr);

    void init() override;

    void start() override;
    void stop() override;

    void processPress(QGraphicsItem* pItem) override;
    void processMove(QGraphicsItem* pItem, const QPointF& currentPos) override;
    void processRelease(QGraphicsItem* pItem) override;

signals:
    void enabledEdit();
    void disabledEdit();
};

}

#endif // GRAPHEDITOR_H
