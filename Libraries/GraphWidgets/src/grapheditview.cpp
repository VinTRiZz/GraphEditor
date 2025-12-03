#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <Components/CustomQt/ObjectView/InternalScene.h>

#include <GraphItems/GraphItemsConstants.h>
#include <GraphItems/PluginObjectInterface.h>
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
    currentGraphObject.clearVertices();

    for (auto [itemId, pItem] : getObjects()) {
        switch (pItem->getObjectType())
        {
        case Graph::OBJECTTYPE_VERTEX:
            currentGraphObject.updateVertex(static_cast<Graph::VertexItem*>(pItem)->toVertex());
            break;

        case Graph::OBJECTTYPE_CONNECTION:
            currentGraphObject.addConnection(static_cast<Graph::VertexConnectionItem*>(pItem)->toConnection());
            break;
        }
    }
    LOG_OK("Graph data write succeed");
}

void GraphEditView::addPendingConnection(Graph::VertexConnectionItem *pCon)
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
    auto topItems = getItems(e->pos(), true);
    std::list<VertexConnectionItem*> connections;
    for (auto* pItem : topItems) {
        if (auto pVertex = dynamic_cast<VertexItem*>(pItem); pVertex != nullptr) {
            for (auto* pCon : m_pendingConnections) {
                pVertex->subscribeAsConnectionTo(pCon);
            }
            break;
        }
    }
    OVLayers::ObjectView::mousePressEvent(e);
}

void GraphEditView::mouseMoveEvent(QMouseEvent *e)
{
    for (auto* pCon : m_pendingConnections) {
        pCon->getLineItem()->setPositionTo(mapToScene(e->pos()));
    }
    OVLayers::ObjectView::mouseMoveEvent(e);
}
