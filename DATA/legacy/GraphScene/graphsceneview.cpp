#include "graphsceneview.h"

#include <AppInfrastructure/GraphEditorSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/Constants.h>

#include <GraphItems/VertexObjectItem.h>
#include <GraphItems/VertexConnectionItem.h>
#include <GraphItems/SceneItemConverter.h>

namespace Graph {

GraphSceneView::GraphSceneView(QWidget* parent) : ObjectView(parent) {
    m_buttonMatrixHead = new ButtonMatrix::HeadButton(this);

    m_buttonMatrixHead->setButtonsSize(QSize(50, 50));
    m_buttonMatrixHead->setButtonMatrix(-10, 0, 0, 10);
    m_buttonMatrixHead->setButtonMargin(10);

    m_buttonMatrixHead->setAnimationSpeed(0.4);
    m_buttonMatrixHead->setIcons(
        QIcon(":/common/images/icons/common/tools_open.svg"),
        QIcon(":/common/images/icons/common/tools_close.svg"));
    m_buttonMatrixHead->setButtonPadding(0, 30, 0, 30);
    m_buttonMatrixHead->collapse(false);
    m_buttonMatrixHead->hide();

    setSceneBrush(GraphEditorSettings::getInstance()
                      .getCanvasConfig()
                      .m_canvasColor);
    setCanvasRect(QRectF(0, 0, 2000, 2000));
    customZoom(1.0 / 200.0);
}

GraphSceneView::~GraphSceneView() {}

void GraphSceneView::setMode(GraphModeBase* pMode) {
    if (nullptr == pMode) [[unlikely]] {
        throw std::invalid_argument("Invalid mode set (nullptr)");
    }

    if (nullptr != m_pCurrentMode) {
        disconnect(m_pCurrentMode, nullptr, this, nullptr);
        disconnect(this, nullptr, m_pCurrentMode, nullptr);
        m_pCurrentMode->stop();
    }

    m_pCurrentMode = pMode;
    if (nullptr == pMode) {
        return;
    }

    pMode->setGraphScene(this);

    connect(this, &ObjectView::pressedOnItem, m_pCurrentMode,
            &GraphModeBase::processPress);
    connect(this, &ObjectView::mouseMoved, m_pCurrentMode,
            &GraphModeBase::processMove);
    connect(this, &ObjectView::releasedOnItem, m_pCurrentMode,
            &GraphModeBase::processRelease);

    m_buttonMatrixHead->show();
    pMode->start();
    LOG_INFO("Changed interaction mode");
}

void GraphSceneView::writeChangesToGraph() {
    auto objects = getAllObjects();
    auto pMaintainer = getGraphMaintaner();
    SceneItemConverter::toMaintainer(pMaintainer, objects);
    LOG_INFO("Graph data cached");
}

void GraphSceneView::setGraphMaintaner(
    const Graph::PMaintainer& pGraphMaintaner) {
    m_pGraphMaintaner = pGraphMaintaner;
    updateGraph();
}

Graph::PMaintainer GraphSceneView::getGraphMaintaner() const {
    return m_pGraphMaintaner;
}

ButtonMatrix::HeadButton* GraphSceneView::getButtonMatrixHead() const {
    return m_buttonMatrixHead;
}

void GraphSceneView::updateGraph() {
    if (!m_pGraphMaintaner) {
        LOG_WARNING("Invalid graph to update data");
        return;
    }

    removeSpecialObjects(ObjectItems::ObjectType(OBJECTTYPE_VERTEX));
    removeSpecialObjects(ObjectItems::ObjectType(OBJECTTYPE_CONNECTION));
    removeSpecialObjects(ObjectItems::OBJECTTYPE_ARROWLINE);

    double labelHeight{0};

    for (auto* pItem :
         SceneItemConverter::fromMaintainer(getGraphMaintaner())) {
        addObject(pItem);
    }
    LOG_INFO("Graph data updated");
}

Graph::VertexConnectionLine* GraphSceneView::createConnectionLine(
    ObjectItems::objectId_t idFrom,
    ObjectItems::objectId_t idTo) {
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }

    auto pConnection = new Graph::VertexConnectionLine;
    pConnection->setObjectId(m_currentItemId);
    addObject(pConnection);

    return pConnection;
}

Graph::VertexObjectItem* GraphSceneView::createVertex() {
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }
    return createVertex(m_currentItemId);
}

Graph::VertexObjectItem* GraphSceneView::createVertex(
    ObjectItems::objectId_t vId) {
    if (!isIdAvailable(vId)) {
        LOG_ERROR("Got unavailable id:", vId);
        return nullptr;
    }

    auto pVertexItem = new Graph::VertexObjectItem;
    pVertexItem->setObjectId(vId);

    pVertexItem->setDisplayName("My node");
    pVertexItem->setToolTip("My node template");
    pVertexItem->setDescription("My example description");

    addObject(pVertexItem);
    return pVertexItem;
}

void GraphSceneView::resizeEvent(QResizeEvent* e) {
    m_buttonMatrixHead->fixPositions();
    ObjectView::resizeEvent(e);
}

}  // namespace Graph
