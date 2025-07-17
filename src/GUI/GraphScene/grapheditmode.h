#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include "graphmodebase.h"

#include "GUI/ObjectScene/predefinedobjects.h"

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
    CurrentEditMode m_currentEditMode {CurrentEditMode::CEM_NONE};

    void clearMode();

    PredefinedObjects::VertexObject*            m_movingVertex {nullptr};
    PredefinedObjects::VertexConnectionLine*    m_movingConnectionLine {nullptr};
    void toggleMovingItem(QGraphicsItem* pItem);
    void clearMovingMode();

    PredefinedObjects::VertexConnectionLine* m_pendingConnectionLine {nullptr};
    bool m_isAddingConnection {false};
    void setPendingConnection(QGraphicsItem* pTargetVertexItem);
    void clearConnectionAddMode();
};

}

#endif // GRAPHEDITOR_H
