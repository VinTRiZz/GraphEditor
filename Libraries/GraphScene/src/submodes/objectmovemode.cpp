#include "objectmovemode.h"

#include "graphsceneview.h"

namespace Graph {

ObjectMoveMode::ObjectMoveMode(GraphModeBase* pParentMode)
    : GraphSubmodeBase(pParentMode) {
    auto& buttonConf = getStarterButton();
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_move.svg");
    buttonConf.secondIcon =
        QIcon(":/common/images/icons/editmode/mode_edit_move_active.svg");
    buttonConf.tooltip = "Перемещение вершин графа";
    buttonConf.positionX = -4;
    buttonConf.positionY = 0;
}

void ObjectMoveMode::clearMode() {
    auto pScene = getParentMode()->getScene();
    if (nullptr != pScene->getGrabObject()) {
        pScene->rejectGrabObject();
    }

    if (nullptr != m_movingVertex) {
        m_movingVertex->updateConnectionLines();
        m_movingVertex->setSelected(false);
        m_movingVertex = nullptr;
    }

    if (nullptr != m_movingConnectionLine) {
        m_movingConnectionLine->resetPositions();
        m_movingConnectionLine->setSelected(false);
        m_movingConnectionLine = nullptr;
    }
}

void ObjectMoveMode::processPress(QGraphicsItem* pTargetItem) {}

void ObjectMoveMode::processMove(QGraphicsItem* pTargetItem,
                                 const QPointF& currentPos) {
    if (nullptr != m_movingConnectionLine) {
        m_movingConnectionLine->getLineItem()->setPositionTo(currentPos);
    }
}

void ObjectMoveMode::processRelease(QGraphicsItem* pTargetItem) {
    auto pItem = dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem);
    if (pItem == nullptr) {
        return;
    }

    auto pScene = getParentMode()->getScene();

    // Если соединение, перемещаем точку целевую
    if (pItem->getType() == OBJECTTYPE_CONNECTION &&
        pItem != m_movingConnectionLine) {
        pItem->setSelected(true);
        m_movingConnectionLine =
            static_cast<Graph::VertexConnectionLine*>(pItem);
        return;
    }

    // Для соединений -- применить изменения
    if (nullptr != m_movingConnectionLine) {
        // Отменяем если не вершина
        if (pItem->getType() != OBJECTTYPE_VERTEX ||
            pItem == m_movingConnectionLine->getVertexFrom()) {
            m_movingConnectionLine->resetPositions();
            clearMode();
            return;
        }

        // Соединяем
        static_cast<Graph::VertexObjectItem*>(pItem)
            ->subscribeAsConnectionTo(m_movingConnectionLine);

        // Забываем, что соединяли только что. Теперь это не наша забота
        clearMode();
        return;
    }

    // Если вершина, прикрепляем её к курсору
    if (pItem->getType() == OBJECTTYPE_VERTEX &&
        pItem != m_movingVertex) {
        if (nullptr != m_movingVertex) {
            pScene->rejectGrabObject();
        }
        pScene->setGrabObject(pItem);
        m_movingVertex = static_cast<Graph::VertexObjectItem*>(pItem);
        return;
    }

    // Забываем, что делали только что (по сути применяем изменения). Теперь это
    // не наша забота
    if (nullptr != m_movingVertex) {
        pScene->acceptGrabObject();
        clearMode();
    }
}

}  // namespace Graph
