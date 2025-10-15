#include "addlinemode.h"

#include "graphsceneview.h"

namespace Graph {

AddLineMode::AddLineMode(GraphModeBase* pParentMode)
    : GraphSubmodeBase(pParentMode) {
    auto& buttonConf = getStarterButton();
    buttonConf.icon =
        QIcon(":/common/images/icons/editmode/mode_edit_add_connection.svg");
    buttonConf.secondIcon = QIcon(
        ":/common/images/icons/editmode/mode_edit_add_connection_active.svg");
    buttonConf.tooltip = "Добавление соединений";
    buttonConf.positionX = -2;
    buttonConf.positionY = 0;
}

void AddLineMode::clearMode() {
    if (nullptr != m_pendingConnectionLine) {
        getParentMode()->getScene()->removeObject(
            m_pendingConnectionLine->getObjectId());
    }
    m_pendingConnectionLine = nullptr;
}

void AddLineMode::processPress(QGraphicsItem* pTargetItem) {}

void AddLineMode::processMove(QGraphicsItem* pTargetItem,
                              const QPointF& currentPos) {
    if (nullptr != m_pendingConnectionLine) {
        m_pendingConnectionLine->setPositionTo(currentPos);
    }
}

void AddLineMode::processRelease(QGraphicsItem* pTargetItem) {
    auto pTargetVertexItem =
        dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem);
    if (pTargetVertexItem == nullptr) {
        return;
    }

    auto pScene = getParentMode()->getScene();

    if (pTargetVertexItem->getType() !=
        ObjectViewItems::OBJECTTYPE_VERTEX) {
        clearMode();
        return;
    }

    // Выбираем начальную точку соединения
    if (nullptr == m_pendingConnectionLine) {
        m_pendingConnectionLine =
            pScene->createConnectionLine(pTargetVertexItem->getObjectId(), 0);

        static_cast<ObjectViewItems::VertexObject*>(pTargetVertexItem)
            ->subscribeAsConnectionFrom(m_pendingConnectionLine);

        m_pendingConnectionLine->show();
        m_pendingConnectionLine->setPositionTo(pTargetVertexItem->pos());
        return;
    }

    auto pCastedVertex =
        static_cast<ObjectViewItems::VertexObject*>(pTargetVertexItem);

    // Нельзя соединять с самой собой
    if (pCastedVertex == m_pendingConnectionLine->getVertexFrom() ||
        pCastedVertex->isLineSubscribed(m_pendingConnectionLine)) {
        clearMode();
        return;
    }

    // Соединяем
    pCastedVertex->subscribeAsConnectionTo(m_pendingConnectionLine);
    m_pendingConnectionLine = nullptr;  // Теперь эта линия не удалится
    clearMode();
}

}  // namespace Graph
