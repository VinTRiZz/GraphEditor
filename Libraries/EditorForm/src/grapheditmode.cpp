#include "grapheditmode.h"

#include "graphsceneview.h"

#include <Common/Logging.h>

namespace Graph
{

GraphEditMode::GraphEditMode(QWidget *parent) :
    GraphModeBase(parent)
{

}

GraphEditMode::~GraphEditMode()
{
    delete m_vertexEditorForm; // Т.к. нету у него parent
}

void GraphEditMode::init()
{
    ButtonMatrix::ButtonConfig buttonConf;

    buttonConf = {};
    buttonConf.icon         = QIcon(":/common/images/icons/editmode/mode_edit_move.svg");
    buttonConf.secondIcon   = QIcon(":/common/images/icons/editmode/mode_edit_move_active.svg");
    buttonConf.tooltip = "Перемещение вершин графа";
    buttonConf.action = [this, buttonConf](QPushButton* pButton) -> void {
        clearMode();
        m_currentEditMode = CEM_MOVING;
        pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -4;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);


    buttonConf = {};
    buttonConf.icon = QIcon(":/common/images/icons/editmode/mode_edit_add_vertex.svg");
    buttonConf.secondIcon   = QIcon(":/common/images/icons/editmode/mode_edit_add_vertex_active.svg");
    buttonConf.tooltip = "Добавление вершин графа";
    buttonConf.action = [this, buttonConf](QPushButton* pButton) -> void {
        clearMode();
        m_currentEditMode = CEM_ADD_VERTEX;
        pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -3;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);



    buttonConf = {};
    buttonConf.icon = QIcon(":/common/images/icons/editmode/mode_edit_add_connection.svg");
    buttonConf.secondIcon   = QIcon(":/common/images/icons/editmode/mode_edit_add_connection_active.svg");
    buttonConf.tooltip = "Добавление соединений";
    buttonConf.action = [this, buttonConf](QPushButton* pButton) -> void {
        clearMode();
        m_currentEditMode = CEM_ADD_CONNECTION;
        pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -2;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);


    buttonConf = {};
    buttonConf.icon = QIcon(":/common/images/icons/editmode/mode_edit_remove.svg");
    buttonConf.secondIcon   = QIcon(":/common/images/icons/editmode/mode_edit_remove_active.svg");
    buttonConf.tooltip = "Удаление элементов графа";
    buttonConf.action = [this, buttonConf](QPushButton* pButton) -> void {
        clearMode();
        m_currentEditMode = CEM_REMOVING;
        pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = -1;
    buttonConf.positionY = 0;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);


    buttonConf = {};
    buttonConf.icon = QIcon(":/common/images/icons/editmode/mode_edit_properties.svg");
    buttonConf.secondIcon   = QIcon(":/common/images/icons/editmode/mode_edit_properties_active.svg");
    buttonConf.tooltip = "Изменение свойств вершины";
    buttonConf.action = [this, buttonConf](QPushButton* pButton) -> void {
        clearMode();
        m_currentEditMode = CEM_EDIT_PROPERTIES;
        pButton->setIcon(buttonConf.secondIcon);
    };
    buttonConf.positionX = 0;
    buttonConf.positionY = 1;
    buttonConf.isEnabled = true;
    m_editButtons.push_back(buttonConf);

    m_propertyEditor = new PropertyEditItem;
    m_vertexEditorForm = new ObjectPropertyEditorForm(nullptr);
    m_propertyEditor->setPropertyEditor(m_vertexEditorForm);
    m_propertyEditor->setSystemId();
    m_propertyEditor->setZValue(ObjectViewConstants::ObjectSceneConfiguration::getInstance().propertyEditorLayer);
    getScene()->addObject(m_propertyEditor);
    m_propertyEditor->hide();

    // Из-за приколов с наследованием
    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (!stylesFile.open(QIODevice::ReadOnly)) {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
        return;
    }
    m_vertexEditorForm->setStyleSheet(stylesFile.readAll());
    LOG_OK("Property editor styles set");
}

void GraphEditMode::start()
{
    auto pButton = getScene()->getButtonMatrixHead();
    for (auto& conf : m_editButtons) {
        pButton->addButton(conf);
    }

    auto& firstButtonConf = m_editButtons.front();
    pButton->getButton(firstButtonConf.positionX, firstButtonConf.positionY)->click();

    setStarted();
}

void GraphEditMode::stop()
{
    clearMode();
    auto pButton = getScene()->getButtonMatrixHead();
    pButton->collapse(false);
    for (auto& conf : m_editButtons) {
        pButton->removeButton(conf.positionX, conf.positionY);
    }
    setStopped();
}

void GraphEditMode::processPress(QGraphicsItem *pItem)
{
    auto pCastedItem = dynamic_cast<ObjectViewItems::ItemBase*>(pItem);

    switch (m_currentEditMode)
    {
    case CEM_NONE:
        // Do nothing (nothing to do)
        break;

    case CEM_MOVING:
        toggleMovingItem(pCastedItem);
        break;

    case CEM_ADD_VERTEX:
        setPendingVertex(pCastedItem);
        break;

    case CEM_ADD_CONNECTION:
        setPendingConnection(pCastedItem);
        break;

    case CEM_REMOVING:
        if (nullptr == pCastedItem) {
            return;
        }
        getScene()->removeObject(pCastedItem->getObjectId());
        break;

    case CEM_EDIT_PROPERTIES:
        setTargetForPropertyEditor(pCastedItem);
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
    case CEM_NONE:
        // Do nothing (nothing to clear)
        break;

    case CEM_MOVING:
        clearMovingMode();
        break;

    case CEM_ADD_VERTEX:
        clearVertexAddMode();
        break;

    case CEM_ADD_CONNECTION:
        clearConnectionAddMode();
        break;

    case CEM_REMOVING:
        // Do nothing (nothing to clear)
        break;

    case CEM_EDIT_PROPERTIES:
        clearPropertyEditMode();
        break;
    }

    auto pButtonMatrixHead = this->getScene()->getButtonMatrixHead();
    for (auto& buttonConf : m_editButtons) {
        auto pButton = pButtonMatrixHead->getButton(buttonConf.positionX, buttonConf.positionY);
        pButton->setIcon(buttonConf.icon);
    }
}

void GraphEditMode::toggleMovingItem(ObjectViewItems::ItemBase *pItem)
{
    if (pItem == nullptr) {
        return;
    }

    auto pScene = getScene();

    // Если соединение, перемещаем точку целевую
    if (pItem->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION &&
        pItem != m_movingConnectionLine) {
        pItem->setSelected(true);
        m_movingConnectionLine = static_cast<ObjectViewItems::VertexConnectionLine*>(pItem);
        pScene->setMovingCallback([this](const QPointF& p) {
            m_movingConnectionLine->setPositionTo(p);
        });
        return;
    }

    // Для соединений -- применить изменения
    if (nullptr != m_movingConnectionLine) {
        // Отменяем если не вершина
        if (pItem->getType() != ObjectViewConstants::OBJECTTYPE_VERTEX ||
            pItem == m_movingConnectionLine->getVertexFrom()) {
            m_movingConnectionLine->resetPositions();
            clearMovingMode();
            return;
        }

        // Соединяем
        static_cast<ObjectViewItems::VertexObject*>(pItem)->subscribeAsConnectionTo(m_movingConnectionLine);

        // Забываем, что соединяли только что. Теперь это не наша забота
        clearMovingMode();
        return;
    }

    // Если вершина, прикрепляем её к курсору
    if (pItem->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX &&
        pItem != m_movingVertex) {
        if (nullptr != m_movingVertex) {
            pScene->rejectGrabObject();
        }
        pScene->setGrabObject(pItem);
        m_movingVertex = static_cast<ObjectViewItems::VertexObject*>(pItem);
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

void GraphEditMode::setPendingConnection(ObjectViewItems::ItemBase *pTargetVertexItem)
{
    if (pTargetVertexItem == nullptr) {
        return;
    }

    auto pScene = getScene();

    if (pTargetVertexItem->getType() != ObjectViewConstants::OBJECTTYPE_VERTEX) {
        clearConnectionAddMode();
        return;
    }

    // Выбираем начальную точку соединения
    if (nullptr == m_pendingConnectionLine) {
        m_pendingConnectionLine = pScene->createConnectionLine(pTargetVertexItem->getObjectId(), 0);

        static_cast<ObjectViewItems::VertexObject*>(pTargetVertexItem)->subscribeAsConnectionFrom(m_pendingConnectionLine);

        pScene->setMovingCallback([this](const QPointF& p) {
            m_pendingConnectionLine->setPositionTo(p);
        });
        m_pendingConnectionLine->show();
        return;
    }

    auto pCastedVertex = static_cast<ObjectViewItems::VertexObject*>(pTargetVertexItem);

    // Нельзя соединять с самой собой
    if (pCastedVertex == m_pendingConnectionLine->getVertexFrom() ||
        pCastedVertex->isLineSubscribed(m_pendingConnectionLine)) {
        clearConnectionAddMode();
        return;
    }

    // Соединяем
    pCastedVertex->subscribeAsConnectionTo(m_pendingConnectionLine);
    m_pendingConnectionLine = nullptr; // Теперь эта линия не удалится
    clearConnectionAddMode();
}

void GraphEditMode::clearConnectionAddMode()
{
    getScene()->setMovingCallback({});
    if (nullptr != m_pendingConnectionLine) {
        getScene()->removeObject(m_pendingConnectionLine->getObjectId());
    }
    m_pendingConnectionLine = nullptr;
}

void GraphEditMode::setPendingVertex(ObjectViewItems::ItemBase *pItem)
{
    auto pScene = getScene();
    if (nullptr != m_pendingVertex) {
        pScene->acceptGrabObject();
        m_pendingVertex = nullptr;
        return;
    }
    m_pendingVertex = pScene->createVertex();
    m_pendingVertex->setPos(pScene->mapToScene(pScene->cursor().pos()));
    pScene->setGrabObject(m_pendingVertex);
}

void GraphEditMode::clearVertexAddMode()
{
    if (nullptr == m_pendingVertex) {
        return;
    }
    auto pScene = getScene();
    if (nullptr != pScene->getGrabObject()) {
        pScene->rejectGrabObject();
    }
    pScene->removeObject(m_pendingVertex->getObjectId());
    m_pendingVertex = nullptr;
}

void GraphEditMode::setTargetForPropertyEditor(ObjectViewItems::ItemBase *pItem)
{
    if (pItem == nullptr) {
        m_propertyEditor->hide();
        return;
    }

    // Игнорируем это, пусть пользователь редактирует свойства
    if (pItem->getType() == ObjectViewConstants::OBJECTTYPE_PROPERTY_EDITOR) {
        return;
    }

    m_propertyEditor->show();
    m_propertyEditor->setTargetItem(pItem);
    m_propertyEditor->setPos(pItem->pos() + QPointF(100, 100));
}

void GraphEditMode::clearPropertyEditMode()
{
    m_propertyEditor->hide();
}

}
