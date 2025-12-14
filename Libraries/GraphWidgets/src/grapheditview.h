#pragma once

#include <Components/CustomQt/ObjectView/ObjectView.h>

#include <GraphObject/GraphObject.h>

namespace Graph {
class VertexConnectionItem;
}

class GraphEditView final :
        public OVLayers::ObjectView,
        public Graph::GraphObjectUser {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

    void writeChanges(); // TODO: Добавить обратное действие

private:
    Graph::VertexConnectionItem* m_pendingConnection {nullptr};

    QMenu m_contextMenu;
    QMenu* createConnectionsMenu();
    QMenu* createGroupsMenu();
    QMenu* createSelectionMenu();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* e) override;
};
