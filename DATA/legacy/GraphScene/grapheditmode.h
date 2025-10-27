#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "graphmodebase.h"

namespace Graph {

class PropertyEditMode;

class GraphEditMode : public GraphModeBase {
    Q_OBJECT
public:
    GraphEditMode(QWidget* parent = nullptr);
    ~GraphEditMode();

    void init() override;

    PropertyEditMode* getPropertyEditMode();

signals:
    void enabledEdit();
    void disabledEdit();
};

}  // namespace Graph

#endif  // GRAPHEDITOR_H
