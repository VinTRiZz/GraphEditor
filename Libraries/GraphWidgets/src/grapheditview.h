#pragma once

#include <Components/CustomQt/ObjectView/ObjectView.h>

#include <GraphObject/Maintainer.h>

namespace Graph {
class VertexConnectionItem;
}

class GraphEditView final :
        public OVLayers::ObjectView,
        public Graph::MaintainerUserDecorator {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

    void writeChanges(); // TODO: Добавить обратное действие

private:
    void addPendingConnection(Graph::VertexConnectionItem* pCon);

    std::list<Graph::VertexConnectionItem*> m_pendingConnections;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
};
