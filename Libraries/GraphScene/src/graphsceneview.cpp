#include "graphsceneview.h"

#include <Common/ApplicationSettings.h>
#include <Common/Logging.h>
#include <GraphObject/Object.h>
#include <ObjectItems/Constants.h>

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

    setSceneBrush(ApplicationSettings::getInstance()
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

    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_VERTEX);
    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION);
    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_ARROWLINE);

    auto& sceneConfig =
        ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    double labelHeight{0};

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);

    for (auto* pItem :
         SceneItemConverter::fromMaintainer(getGraphMaintaner())) {
        addObject(pItem);
    }
    LOG_INFO("Graph data updated");
}

ObjectViewItems::VertexConnectionLine* GraphSceneView::createConnectionLine(
    ObjectViewConstants::objectId_t idFrom,
    ObjectViewConstants::objectId_t idTo) {
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }

    auto pConnection = new ObjectViewItems::VertexConnectionLine;
    pConnection->setObjectId(m_currentItemId);
    pConnection->setZValue(
        ObjectViewConstants::ObjectSceneConfiguration::getInstance()
            .connectionLineLayer);
    addObject(pConnection);

    return pConnection;
}

ObjectViewItems::VertexObject* GraphSceneView::createVertex() {
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }
    return createVertex(m_currentItemId);
}

ObjectViewItems::VertexObject* GraphSceneView::createVertex(
    ObjectViewConstants::objectId_t vId) {
    if (!isIdAvailable(vId)) {
        LOG_ERROR("Got unavailable id:", vId);
        return nullptr;
    }

    auto pVertexItem = new ObjectViewItems::VertexObject;
    pVertexItem->setObjectId(vId);

    pVertexItem->setDisplayName("My node");
    pVertexItem->setToolTip("My node template");
    pVertexItem->setDescription("My example description");

    auto& sceneConfig =
        ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    pVertexItem->setZValue(sceneConfig.vertexLayer);

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);
    pVertexItem->setRect(vertexRect);

    addObject(pVertexItem);
    return pVertexItem;
}

void GraphSceneView::resizeEvent(QResizeEvent* e) {
    m_buttonMatrixHead->fixPositions();
    ObjectView::resizeEvent(e);
}

}  // namespace Graph
