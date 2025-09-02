#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"
#include "graphmodebase.h"

namespace Graph {

class GraphEditMode : public GraphModeBase {
    Q_OBJECT
public:
    GraphEditMode(QWidget* parent = nullptr);
    ~GraphEditMode();

    void init() override;

signals:
    void enabledEdit();
    void disabledEdit();

    void openPropertyEditor(QGraphicsItem* pTargetItem);
    void closePropertyEditor();
};

}  // namespace Graph

#endif  // GRAPHEDITOR_H
