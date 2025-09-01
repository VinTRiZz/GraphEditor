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

private:
    std::list<ButtonMatrix::ButtonConfig> m_editButtons;

    // Флаги редактирования
    enum CurrentEditMode {
        CEM_NONE,
        CEM_MOVING,
        CEM_ADD_VERTEX,
        CEM_ADD_CONNECTION,
        CEM_EDIT_PROPERTIES,
        CEM_REMOVING,
    };
    CurrentEditMode m_currentEditMode{CurrentEditMode::CEM_NONE};

    void clearMode();

    ObjectViewItems::VertexConnectionLine* m_pendingConnectionLine{nullptr};
    void setPendingConnection(ObjectViewItems::ItemBase* pTargetVertexItem);
    void clearConnectionAddMode();

    ObjectViewItems::VertexObject* m_pendingVertex{nullptr};
    void setPendingVertex(ObjectViewItems::ItemBase* pItem);
    void clearVertexAddMode();

    void setTargetForPropertyEditor(ObjectViewItems::ItemBase* pItem);
    void clearPropertyEditMode();
};

}  // namespace Graph

#endif  // GRAPHEDITOR_H
