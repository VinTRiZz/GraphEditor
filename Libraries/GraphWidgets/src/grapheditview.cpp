#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <Components/CustomQt/ObjectView/InternalScene.h>

#include <GraphItems/GraphItemsConstants.h>
#include <GraphObject/PluginObjectInterface.h>
#include <GraphItems/VertexItem.h>
#include <GraphItems/VertexConnectionItem.h>

#include <PluginModule/PluginMaster.h>
#include <PluginCoreInterface/Core.h>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>

#include <QMimeData>

#include "graphwidgetconstants.hpp"

using namespace Graph;

GraphEditView::GraphEditView(QWidget* parent) :
    OVLayers::ObjectView(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));
}

void GraphEditView::writeChanges()
{
    LOG_INFO("Reading graph...");
    auto currentGraphObject = getGraph()->getObject();
    currentGraphObject->clearVertices();

    for (auto [itemId, pItem] : getObjects()) {
        switch (pItem->getObjectType())
        {
        case Graph::OBJECTTYPE_VERTEX:
            currentGraphObject->updateVertex(static_cast<Graph::VertexItem*>(pItem)->toGObject());
            break;

        case Graph::OBJECTTYPE_CONNECTION:
            // TODO: Fix connection adding
//            currentGraphObject->addConnection(static_cast<Graph::VertexConnectionItem*>(pItem)->toConnection());
            break;
        }
    }
    LOG_OK("Graph data write succeed");
}

QMenu *GraphEditView::createConnectionsMenu()
{
    auto pMenu = new QMenu("Соединить", this);

    auto pAction = new QAction("Напрямую", pMenu);
    pMenu->addAction(pAction);

    pAction = new QAction("Фигурно", pMenu);
    pMenu->addAction(pAction);

    return pMenu;
}

QMenu *GraphEditView::createGroupsMenu()
{
    auto pMenu = new QMenu("Группа", this);

    auto pAction = new QAction("Создать", pMenu);
    pMenu->addAction(pAction);

    pAction = new QAction("Добавить в группу", pMenu);
    pMenu->addAction(pAction);

    pAction = new QAction("Исключить из группы", pMenu);
    pMenu->addAction(pAction);

    pAction = new QAction("Удалить группу", pMenu);
    pMenu->addAction(pAction);

    return pMenu;
}

QMenu *GraphEditView::createSelectionMenu()
{

}

void GraphEditView::dragEnterEvent(QDragEnterEvent *event)
{
    OVLayers::ObjectView::updateCursorLabel();
    if (event->mimeData()->hasFormat(Graph::GraphWidgetConstants::MIMETYPE_PLUGINOBJECT)) {
        event->acceptProposedAction();
    }
}

void GraphEditView::dragMoveEvent(QDragMoveEvent *event)
{
    OVLayers::ObjectView::updateCursorLabel();
    if (event->mimeData()->hasFormat(Graph::GraphWidgetConstants::MIMETYPE_PLUGINOBJECT)) {
        event->acceptProposedAction();
    }
}

void GraphEditView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(Graph::GraphWidgetConstants::MIMETYPE_PLUGINOBJECT)) {
        QString itemInfo = event->mimeData()->data(Graph::GraphWidgetConstants::MIMETYPE_PLUGINOBJECT);

        auto itemInfoSplitted = itemInfo.split("\n");
        if (itemInfoSplitted.size() < 2) {
            LOG_WARNING("GraphEditView: Invalid data in drop event");
            return;
        }
        auto itemPlugin = itemInfoSplitted.at(0);
        auto itemName = itemInfoSplitted.at(1);
        LOG_INFO("Loading item", itemName, "from plugin", itemPlugin);

        auto& pluginMaster = PluginMaster::getInstance();
        auto pPlugin = pluginMaster.getPlugin(itemPlugin);
        auto pInterface = pPlugin->getPluginCore()->createObject(itemName);
        auto pItem = dynamic_cast<ObjectItems::BasicItem*>(pInterface);
        if (nullptr != pItem) {
            pItem->setPos(mapToScene(event->pos()) - pItem->boundingRect().center());
            addObject(pItem);
            LOG_OK("Added item:", itemName, "from plugin:", itemPlugin);
            event->acceptProposedAction();
        } else {
            delete pInterface;
            LOG_WARNING("Not a graphics item got! Plugin:", itemPlugin, "object name:", itemName);
        }
    }
    OVLayers::ObjectView::updateCursorLabel(); // Непонятный прикол
}

void GraphEditView::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::MouseButton::LeftButton &&
        nullptr != m_pendingConnection) {
        auto topItems = getItems(e->pos(), true);
        std::list<VertexConnectionItem*> connections;
        for (auto* pItem : topItems) {
            if (auto pVertex = dynamic_cast<VertexItem*>(pItem); pVertex != nullptr) {
                pVertex->subscribeAsConnectionTo(m_pendingConnection);
                break;
            }
        }
    }
    OVLayers::ObjectView::mousePressEvent(e);
}

void GraphEditView::mouseMoveEvent(QMouseEvent *e)
{
    if (nullptr != m_pendingConnection) {
        m_pendingConnection->getLineItem()->setPositionTo(mapToScene(e->pos()));
    }
    OVLayers::ObjectView::mouseMoveEvent(e);
}

void GraphEditView::contextMenuEvent(QContextMenuEvent *e)
{
    m_contextMenu.clear();

    auto pHoverItem = getTopItem(e->pos());

    auto pHoverItemObject = dynamic_cast<ObjectItems::BasicItem*>(pHoverItem);
    if (pHoverItemObject != nullptr) {
        m_contextMenu.addMenu(pHoverItemObject->createContextMenu());

        auto itemType = pHoverItemObject->getObjectType();
        switch (itemType)
        {
        case Graph::OBJECTTYPE_VERTEX:
            m_contextMenu.addMenu(createConnectionsMenu());
            m_contextMenu.addAction("Комментарий", [this](){
                LOG_DEBUG("ADD COMMENT, BRO!");
            });
            break;
        }
    }

    auto pGridAction = m_contextMenu.addAction("Сетка", [this]() {
        getScene()->setGridEnabled(!getScene()->getIsGridEnabled());
        for (auto* pAction : m_contextMenu.actions()) {
            if (pAction->text() == "Сетка") {
                pAction->setChecked(getScene()->getIsGridEnabled());
                break;
            }
        }
    });
    pGridAction->setCheckable(true);

    m_contextMenu.addAction("Вернуться в центр", [this](){
        resetScale();
        centerOn(getCanvas()->boundingRect().center());
    });

    // Подразумевается, что это меню не было определено
    m_contextMenu.exec(e->globalPos());
}
