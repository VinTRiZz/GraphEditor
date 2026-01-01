#pragma once

#include <Components/CustomQt/ObjectView/ObjectView.h>

#include <GraphObject/GraphObject.h>

namespace Graph {
class GObjectConnectionItem;
class GObjectItem;
}

class GraphEditView final :
        public OVLayers::ObjectView,
        public Graph::GraphObjectUser {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

private:
    Graph::GObjectConnectionItem* m_pendingConnection {nullptr};

    QMenu m_contextMenu;
    QMenu* createConnectionsMenu(Graph::GObjectItem *hoverVertex);
    QMenu* createGroupsMenu(Graph::GObjectItem *hoverVertex);
    QMenu* createSelectionMenu();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* e) override;
};
