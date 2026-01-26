#pragma once

#include <Components/CustomQt/ObjectView/ObjectView.h>

#include <GraphObject/GraphObject.h>

#include <QJsonArray>

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

    void resetCanvas();
    void setCanvasSize(Graph::CanvasSize sizeType);
    void setCanvasSize(const QSizeF& siz);
    void setCanvasOrientation(Qt::Orientation orient);

private:
    using OVLayers::ObjectView::setCanvasRect; // Скрываем интерфейс

    Graph::CanvasSize m_canvasSize {Graph::CanvasSize::CS_Custom};
    Qt::Orientation m_canvasOrientation {Qt::Horizontal};

    Graph::GObjectConnectionItem* m_pendingConnection {nullptr};

    QMenu m_contextMenu;
    QMenu* createConnectionsMenu(Graph::GObjectItem *hoverVertex);
    QMenu* createSelectionMenu();

    QJsonArray m_serializedItems;

    void connectItem(ObjectItems::BasicItem* pItem);
    bool m_isMovingGroup {false};

    void setupHotkeys();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* e) override;

    void processGraphChange(const Graph::GraphObjectManagerPtr& pPrevGraph) override;
};
