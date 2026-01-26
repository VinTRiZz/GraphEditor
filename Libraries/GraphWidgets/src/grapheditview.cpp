#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <Components/CustomQt/ObjectView/InternalScene.h>
#include <Components/Common/ApplicationSettings.h>

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
    setInformationLabelEnabled(false);
    customZoom(0.5);

    setCursorValuesPresenter([this](const QPointF& curPoint) -> QString {
        auto pObject = getObject(mapFromScene(curPoint));
        QString objectName {};
        QString objectDescription {};
        if (pObject != nullptr) {
            objectName = pObject->getDisplayName();
            objectDescription = pObject->getDescription();
            getCursorLabel()->setMaxSymbolCount(250);
            return objectName + (objectDescription.isEmpty() ? "" : "\n" + objectDescription);
        }

        getCursorLabel()->setMaxSymbolCount(100);
        return QString("(%0; %1)").arg(QString::number(curPoint.x()), QString::number(curPoint.y()));
    });

    auto& appSettings = Common::ApplicationSettings::getInstance();
    auto pSizeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE);
    auto pSizeTypeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE_TYPE);
    auto pOrientationSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_ORIENTATION);
    if (pSizeTypeSetting->getValue().isNull()) {
        resetCanvas();
    } else {
        auto canvasSizeInfo = pSizeTypeSetting->getValue().toString();
        if (canvasSizeInfo == "custom") {
            auto sizeData = pSizeSetting->getValue().toString();
            auto sizeValues = sizeData.split("x");
            if (sizeValues.size() > 1) {
                setCanvasSize(QSizeF(sizeValues[0].toDouble(), sizeValues[1].toDouble()));
            } else {
                setCanvasSize(Graph::CanvasSize::CS_A4);
            }
        } else if (canvasSizeInfo.isEmpty() || canvasSizeInfo.size() != 2) {
            LOG_WARNING("Unknown canvas size type:", canvasSizeInfo, "(try English letters and symbols)");
            resetCanvas();
        } else {
            setCanvasSize(Graph::CanvasSize(canvasSizeInfo.remove(0, 1).toInt() + 1)); // Грязно, но эффективно
        }

        auto canvasOrientation = pOrientationSetting->getValue().toString();
        if (canvasOrientation == "vertical") {
            setCanvasOrientation(Qt::Vertical);
        } else if (canvasOrientation == "horizontal") {
            setCanvasOrientation(Qt::Horizontal);
        } else {
            LOG_WARNING("Unknown orientation:", canvasOrientation, "(try \"vertical\" or \"horizontal\")");
        }
    }
}

void GraphEditView::resetCanvas()
{
    setCanvasSize(Graph::CanvasSize::CS_A4);
    setCanvasOrientation(Qt::Vertical);
}

void GraphEditView::setCanvasSize(Graph::CanvasSize sizeType)
{
    m_canvasSize = sizeType;

    auto& appSettings = Common::ApplicationSettings::getInstance();
    auto pSizeTypeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE_TYPE);

    if (m_canvasSize == Graph::CanvasSize::CS_Custom) {
        pSizeTypeSetting->setValue("custom");
        return;
    }

    if (Graph::CANVAS_SIZE.count(sizeType) == 0) {
        throw std::runtime_error("Invalid size type");
    }
    auto targetSize = Graph::CANVAS_SIZE.at(sizeType);

    // TODO: Вынести в скейл фактор или что-то такого рода
    targetSize.setWidth(targetSize.width() * 10.0);
    targetSize.setHeight(targetSize.height() * 10.0);

    // Поворачиваем в альбомную
    if (m_canvasOrientation == Qt::Horizontal) {
        auto sizeW = targetSize.width();
        targetSize.setWidth(targetSize.height());
        targetSize.setHeight(sizeW);
    }

    setCanvasRect(QRectF(0, 0, targetSize.width(), targetSize.height()));

    pSizeTypeSetting->setValue(QString("A%0").arg(int(m_canvasSize - 1)));

    auto pSizeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE);
    pSizeSetting->setValue({});
}

void GraphEditView::setCanvasSize(const QSizeF &siz)
{
    setCanvasSize(Graph::CanvasSize::CS_Custom);
    setCanvasRect(QRectF(0, 0, siz.width(), siz.height()));

    auto& appSettings = Common::ApplicationSettings::getInstance();
    auto pSizeTypeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE_TYPE);
    pSizeTypeSetting->setValue("custom");

    auto pSizeSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_SIZE);
    pSizeSetting->setValue(QString("%0x%1").arg(QString::number(siz.width()), QString::number(siz.height())));
}

void GraphEditView::setCanvasOrientation(Qt::Orientation orient)
{
    m_canvasOrientation = orient;
    auto& appSettings = Common::ApplicationSettings::getInstance();
    auto pOrientationSetting = appSettings.getSetting(Graph::SettingsNames::CANVASCONFIG, Graph::SettingsNames::CANVASCONFIG_ORIENTATION);
    pOrientationSetting->setValue(m_canvasOrientation == Qt::Vertical ? "vertical" : "horizontal");

    setCanvasSize(m_canvasSize); // Обновляем
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

        auto pConLine = new StraightConnection;
        pConLine->setItemId(getFreeObjectId());
        pConLine->setDirection(ObjectItems::LineDirectionType::Forward);
        pConLine->setArrowFilled(true);

        auto& appSettings = Common::ApplicationSettings::getInstance();
        pConLine->setLinePen(Colors::DEFAULT_COLOR_CONNECTION_LINE);
        pConLine->setLineSelectionPen(Colors::DEFAULT_COLOR_CONNECTION_SEL);
        m_pendingConnection->setLineItem(pConLine);

        m_pendingConnection->setVertexFrom(hoverVertex);
        m_pendingConnection->getLineItem()->setPositionTo(mapToScene(mapFromGlobal(cursor().pos())));
        getGraph()->getObject()->addPluginObject(m_pendingConnection);
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

        auto pConLine = new ElegantConnection;
        pConLine->setItemId(getFreeObjectId());
        pConLine->setDirection(ObjectItems::LineDirectionType::Forward);
        pConLine->setArrowFilled(true);

        auto& appSettings = Common::ApplicationSettings::getInstance();
        pConLine->setLinePen(Colors::DEFAULT_COLOR_CONNECTION_LINE);
        pConLine->setLineSelectionPen(Colors::DEFAULT_COLOR_CONNECTION_SEL);
        m_pendingConnection->setLineItem(pConLine);

        m_pendingConnection->setVertexFrom(hoverVertex);
        m_pendingConnection->getLineItem()->setPositionTo(mapToScene(mapFromGlobal(cursor().pos())));
        getGraph()->getObject()->addPluginObject(m_pendingConnection);
    });
    pMenu->addAction(pAction);

    return pMenu;
}

QMenu *GraphEditView::createSelectionMenu()
{
    auto pMenu = new QMenu("Выбор", this);

    auto pAction = new QAction("Объединить в группу", pMenu);
    connect(pAction, &QAction::triggered,
            this, [this](){
        auto pGroup = new ObjectItems::CommentItem;
        pGroup->enableDeleteOnEmpty();
        pGroup->setItemId(getFreeObjectId());
        addObject(pGroup);

        pGroup->setZValue(Layers::VERTEX_LAYER - 1);
        pGroup->setDisplayName("Группа объектов");
        pGroup->setParentItem(getCanvas());

        for (auto* pItem : getScene()->selectedItems()) {
            auto pCastedItem = dynamic_cast<ObjectItems::BasicItem*>(pItem);
            if (nullptr != pCastedItem) {
                pGroup->addGroupItem(pCastedItem);
            }
        }
    });
    pMenu->addAction(pAction);

    return pMenu;
}

void GraphEditView::connectItem(ObjectItems::BasicItem *pItem)
{
    connect(pItem, &ObjectItems::BasicItem::itemAboutToMove,
            this, [this, pItem](auto nPos){
        // Защита от стек оверфлоу
        if (m_isMovingGroup || !pItem->isSelected()) {
            return;
        }
        m_isMovingGroup = true;

        if (nullptr != pItem->parentItem()) {
            nPos = pItem->parentItem()->mapToScene(nPos);
        }
        auto deltaPos = pItem->scenePos() - nPos;
        for (auto* pSelectedItem : getScene()->selectedItems()) {
            if (pSelectedItem == pItem) {
                continue;
            }
            auto deltaSelPos = deltaPos;
            if (nullptr != pSelectedItem->parentItem()) {
                deltaSelPos = pSelectedItem->parentItem()->mapToScene(deltaSelPos);
            }
            pSelectedItem->setPos(pSelectedItem->pos() - deltaSelPos);
        }
        m_isMovingGroup = false;
    });
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
            connectItem(pItem);

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
                m_pendingConnection->setVertexTo(pVertex);
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
    QMenu* pSubmenu {nullptr}; // TODO: Придумать, как удалять по-нормальному
    m_contextMenu.clear();

    auto pHoverItem = getTopItem(e->pos());

    auto pHoverItemObject = dynamic_cast<ObjectItems::BasicItem*>(pHoverItem);
    if ((pHoverItemObject == nullptr) && (nullptr != pHoverItem)) {
        pHoverItemObject = pHoverItem->data(ObjectItems::OBJECTDATAROLE_PARENTITEM_POINTER).value<ObjectItems::BasicItem*>();
    }

    if (pHoverItemObject != nullptr) {
        auto pParent = pHoverItemObject->getParentObject();
        if (nullptr != pParent) {
            pHoverItemObject = pParent;
        }

        pSubmenu = pHoverItemObject->createContextMenu();
        m_contextMenu.addMenu(pSubmenu);

        auto itemType = pHoverItemObject->getObjectType();
        switch (itemType)
        {
        case Graph::OBJECTTYPE_VERTEX:
            m_contextMenu.addMenu(createConnectionsMenu(static_cast<GObjectItem*>(pHoverItemObject)));
            break;
        }

        auto pAction = new QAction("Удалить", &m_contextMenu);
        connect(pAction, &QAction::triggered,
                this, [this, pHoverItemObject](){
            if (getScene()->selectedItems().empty()) {
                removeObject(pHoverItemObject);
            }
            for (auto* pItem : getScene()->selectedItems()) {
                auto pCastedItem = dynamic_cast<ObjectItems::BasicItem*>(pItem);
                if (nullptr != pCastedItem) {
                    removeObject(pCastedItem);
                }
            }
        });
        m_contextMenu.addAction(pAction);
    }

    if (!getScene()->selectedItems().empty()) {
        m_contextMenu.addMenu(createSelectionMenu());
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
    delete pSubmenu;
}

void GraphEditView::processGraphChange(const Graph::GraphObjectManagerPtr& pPrevGraph)
{
    if (pPrevGraph && pPrevGraph->getObject()) {
        auto pPrevObj = pPrevGraph->getObject();
        for (auto* pItem : pPrevObj->getAllObjects()) {
            disconnect(pItem, nullptr, this, nullptr);
        }
        for (auto* pItem : pPrevObj->getPluginObjects()) {
            if (auto pCastedItem = dynamic_cast<ObjectItems::BasicItem*>(pItem); nullptr != pCastedItem) {
                disconnect(pCastedItem, nullptr, this, nullptr);
            }
        }
    }

    auto pCurrentGraph = getGraph();
    if (pCurrentGraph && pCurrentGraph->getObject()) {
        auto pCurrentObj = pCurrentGraph->getObject();
        for (auto* pItem : pCurrentObj->getAllObjects()) {
            connectItem(pItem);
        }
        for (auto* pItem : pCurrentObj->getPluginObjects()) {
            if (auto pCastedItem = dynamic_cast<ObjectItems::BasicItem*>(pItem); nullptr != pCastedItem) {
                connectItem(pCastedItem);
            }
        }
    }
}
