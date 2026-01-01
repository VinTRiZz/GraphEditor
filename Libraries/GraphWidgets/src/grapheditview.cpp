#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <Components/CustomQt/ObjectView/InternalScene.h>


#include <GraphObject/PluginObjectInterface.h>
#include <GraphObject/GraphObject.h>

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

QMenu *GraphEditView::createConnectionsMenu(GObjectItem *hoverVertex)
{
    auto pMenu = new QMenu("Соединить", this);

    auto pAction = new QAction("Напрямую", pMenu);
    connect(pAction, &QAction::triggered,
            this, [this, hoverVertex](){
        if (m_pendingConnection) { removeObject(m_pendingConnection); }
        m_pendingConnection = new GObjectConnectionItem;
        m_pendingConnection->setItemId(getFreeObjectId());
        addObject(m_pendingConnection);
        m_pendingConnection->setParentItem(getCanvas());
        hoverVertex->subscribeAsConnectionFrom(m_pendingConnection);
    });
    pMenu->addAction(pAction);

    pAction = new QAction("Фигурно", pMenu);
    connect(pAction, &QAction::triggered,
            this, [this, hoverVertex](){
        if (m_pendingConnection) { removeObject(m_pendingConnection); }
        m_pendingConnection = new GObjectConnectionItem;
        m_pendingConnection->setItemId(getFreeObjectId());
        addObject(m_pendingConnection);
        m_pendingConnection->setParentItem(getCanvas());
        auto pConLine = new ObjectItems::ElegantConnectionLine;
        pConLine->setDirection(ObjectItems::LineDirectionType::Forward);
        m_pendingConnection->setLineItem(pConLine);
        hoverVertex->subscribeAsConnectionFrom(m_pendingConnection);
    });
    pMenu->addAction(pAction);

    return pMenu;
}

QMenu *GraphEditView::createGroupsMenu(GObjectItem *hoverVertex)
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
    auto pMenu = new QMenu("Выбор", this);

    auto pAction = new QAction("Удалить", pMenu);
    pMenu->addAction(pAction);

    pAction = new QAction("Объединить в группу", pMenu);
    pMenu->addAction(pAction);

    return pMenu;
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
            pItem->setItemId(getFreeObjectId());
            addObject(pItem);
            if (auto pObj = dynamic_cast<GObjectItem*>(pItem); nullptr != pObj) {
                getGraph()->getObject()->addObject(pObj);
            } else {
                getGraph()->getObject()->addPluginObject(pInterface);
            }
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
        std::list<GObjectConnectionItem*> connections;
        for (auto* pItem : topItems) {
            if (auto pVertex = dynamic_cast<GObjectItem*>(pItem); pVertex != nullptr) {
                pVertex->subscribeAsConnectionTo(m_pendingConnection);
                m_pendingConnection = nullptr;
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
            m_contextMenu.addMenu(createConnectionsMenu(static_cast<GObjectItem*>(pHoverItemObject)));
            m_contextMenu.addAction("Комментарий", [this](){
                LOG_DEBUG("ADD COMMENT, BRO!");
            });
            break;
        }
    }

    auto pGridAction = m_contextMenu.addAction("Сетка", [this]() {
        getScene()->setGridEnabled(!getScene()->getIsGridEnabled());
    });
    pGridAction->setCheckable(true);
    pGridAction->setChecked(getScene()->getIsGridEnabled());

    m_contextMenu.addAction("Вернуться в центр", [this](){
        resetScale();
        centerOn(getCanvas()->boundingRect().center());
    });

    m_contextMenu.exec(e->globalPos());
}
