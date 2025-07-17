#include "grapheditmode.h"

#include "graphscenebase.h"

#include "logging.h"

namespace Graph
{

GraphEditMode::GraphEditMode(QWidget *parent) :
    GraphModeBase(parent)
{

}

void GraphEditMode::init()
{
    ButtonMatrix::ButtonConfig buttonConf;

    buttonConf = {};
    buttonConf.icon = QIcon("://DATA/images/icons/mode_edit_move.svg");
    buttonConf.tooltip = "Перемещение вершин графа";
    buttonConf.action = [this](QPushButton*) -> void {
        clearMode();
        m_currentEditMode = CEM_MOVING;
    };
    buttonConf.positionX = -4;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);


    buttonConf = {};
    buttonConf.icon = QIcon("://DATA/images/icons/mode_edit_add_vertex.svg");
    buttonConf.tooltip = "Добавление вершин графа";
    buttonConf.action = [this](QPushButton*) -> void {
        clearMode();
        m_currentEditMode = CEM_ADD_VERTEX;
    };
    buttonConf.positionX = -3;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);



    buttonConf = {};
    buttonConf.icon = QIcon("://DATA/images/icons/mode_edit_add_connection.svg");
    buttonConf.tooltip = "Добавление соединений";
    buttonConf.action = [this](QPushButton*) -> void {
        clearMode();
        m_currentEditMode = CEM_ADD_CONNECTION;
    };
    buttonConf.positionX = -2;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);


    buttonConf = {};
    buttonConf.icon = QIcon("://DATA/images/icons/mode_edit_remove.svg");
    buttonConf.tooltip = "Удаление элементов графа";
    buttonConf.action = [this](QPushButton*) -> void {
        clearMode();
        m_currentEditMode = CEM_REMOVING;
    };
    buttonConf.positionX = -1;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);
}

void GraphEditMode::start()
{
    auto pButton = getScene()->getButtonMatrixHead();
    for (auto& conf : m_editButtons) {
        pButton->addButton(conf);
    }
    setStarted();
}

void GraphEditMode::stop()
{
    auto pButton = getScene()->getButtonMatrixHead();
    pButton->collapse(false);
    for (auto& conf : m_editButtons) {
        pButton->removeButton(conf.positionX, conf.positionY);
    }
    setStopped();
}

void GraphEditMode::processPress(QGraphicsItem *pItem)
{
    switch (m_currentEditMode)
    {
    case CEM_MOVING:
        toggleMovingItem(pItem);
        break;

    case CEM_ADD_CONNECTION:
        setPendingConnection(pItem);
        break;
    }
}

void GraphEditMode::processMove(QGraphicsItem *pItem, const QPointF &currentPos)
{

}

void GraphEditMode::processRelease(QGraphicsItem *pItem)
{

}

void GraphEditMode::clearMode()
{
    switch (m_currentEditMode)
    {
    case CEM_MOVING:
        clearMovingMode();
        break;

    case CEM_ADD_CONNECTION:
        clearConnectionAddMode();
        break;
    }
}

void GraphEditMode::toggleMovingItem(QGraphicsItem *pItem)
{
    auto pScene = getScene();

    // Если соединение, перемещаем точку целевую
    if (pItem->data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt() == GraphSceneBase::OBJECT_TYPE_CONNECTION &&
        pItem != m_movingConnectionLine) {
        m_movingConnectionLine = static_cast<PredefinedObjects::VertexConnectionLine*>(pItem);
        pScene->setMovingCallback([this](const QPointF& p) {
            m_movingConnectionLine->setPositionTo(p);
        });
        return;
    }

    // Для соединений -- применить изменения
    if (nullptr != m_movingConnectionLine) {
        // Отменяем если не вершина
        if (pItem->data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt() != GraphSceneBase::OBJECT_TYPE_VERTEX ||
            pItem == m_movingConnectionLine->getVertexFrom()) {
            m_movingConnectionLine->resetPositions();
            clearMovingMode();
            return;
        }

        // Соединяем
        static_cast<PredefinedObjects::VertexObject*>(pItem)->subscribeAsConnectionTo(m_movingConnectionLine);

        // Забываем, что соединяли только что. Теперь это не наша забота
        clearMovingMode();
        return;
    }

    // Если вершина, прикрепляем её к курсору
    if (pItem->data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt() == GraphSceneBase::OBJECT_TYPE_VERTEX &&
        pItem != m_movingVertex) {
        if (nullptr != m_movingVertex) {
            pScene->rejectGrabObject();
        }
        pScene->setGrabObject(pItem);
        m_movingVertex = static_cast<PredefinedObjects::VertexObject*>(pItem);
        return;
    }

    // Забываем, что делали только что (по сути применяем изменения). Теперь это не наша забота
    if (nullptr != m_movingVertex) {
        pScene->acceptGrabObject();
        clearMovingMode();
    }
}

void GraphEditMode::clearMovingMode()
{
    auto pScene = getScene();
    pScene->setMovingCallback({});

    if (nullptr != m_movingConnectionLine) {
        m_movingConnectionLine->setSelected(false);
        m_movingConnectionLine = nullptr;
    }

    if (nullptr != m_movingVertex) {
        m_movingVertex->setSelected(false);
        m_movingVertex = nullptr;
    }

    if (nullptr == pScene->getGrabObject()) {
        return;
    }
    pScene->rejectGrabObject();
}

void GraphEditMode::setPendingConnection(QGraphicsItem *pTargetVertexItem)
{
    auto pScene = getScene();

    if (pTargetVertexItem->data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt() != GraphSceneBase::OBJECT_TYPE_VERTEX) {
        clearConnectionAddMode();
        return;
    }

    // Выбираем начальную точку соединения
    if (nullptr == m_pendingConnectionLine) {
        m_pendingConnectionLine = pScene->createConnectionLine();

        static_cast<PredefinedObjects::VertexObject*>(pTargetVertexItem)->subscribeAsConnectionFrom(m_pendingConnectionLine);

        pScene->setMovingCallback([this](const QPointF& p) {
            m_pendingConnectionLine->setPositionTo(p);
        });
        m_pendingConnectionLine->show();
        return;
    }

    // Соединяем
    static_cast<PredefinedObjects::VertexObject*>(pTargetVertexItem)->subscribeAsConnectionTo(m_pendingConnectionLine);
    m_pendingConnectionLine = nullptr;
    clearConnectionAddMode();
}

void GraphEditMode::clearConnectionAddMode()
{
    getScene()->setMovingCallback({});
    delete m_pendingConnectionLine;
    m_pendingConnectionLine = nullptr;
}

}
