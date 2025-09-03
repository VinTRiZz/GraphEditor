#include "addobjectmode.h"

#include "graphsceneview.h"

namespace Graph {

AddObjectMode::AddObjectMode(GraphModeBase* pParentMode)
    : GraphSubmodeBase(pParentMode) {
    auto& buttonConf = getStarterButton();
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_add_vertex.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_add_vertex_active.svg");
    buttonConf.tooltip = "Добавление вершин графа";
    buttonConf.positionX = -3;
    buttonConf.positionY = 0;
}

void AddObjectMode::clearMode() {
    if (nullptr == m_pendingVertex) {
        return;
    }
    auto pScene = getParentMode()->getScene();
    if (nullptr != pScene->getGrabObject()) {
        pScene->rejectGrabObject();
    }
    pScene->removeObject(m_pendingVertex->getObjectId());
    m_pendingVertex = nullptr;
}

void AddObjectMode::processPress(QGraphicsItem* pTargetItem) {
    auto pScene = getParentMode()->getScene();
    if (nullptr != m_pendingVertex) {
        pScene->acceptGrabObject();
        m_pendingVertex = nullptr;
        return;
    }
    m_pendingVertex = pScene->createVertex();
    m_pendingVertex->setPos(pScene->mapToScene(pScene->cursor().pos()));
    pScene->setGrabObject(m_pendingVertex);
}

void AddObjectMode::processMove(QGraphicsItem* pTargetItem,
                                const QPointF& currentPos) {}

void AddObjectMode::processRelease(QGraphicsItem* pTargetItem) {}

}  // namespace Graph
