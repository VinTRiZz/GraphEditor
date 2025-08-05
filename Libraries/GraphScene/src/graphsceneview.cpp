#include "graphsceneview.h"

#include <Common/ApplicationSettings.h>
#include <Common/Logging.h>
#include <GraphObject/Object.h>
#include <ObjectItems/Constants.h>

#include "Items/connectionlineitem.h"
#include "Items/vertexobjectitem.h"

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

    setSceneBrush(ApplicationSettings::getInstance().getCanvasConfig().getBackgroundColor());
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

    connect(this, &ObjectView::pressedOnItem, m_pCurrentMode,
            &GraphModeBase::processPress);
    connect(this, &ObjectView::releasedOnItem, m_pCurrentMode,
            &GraphModeBase::processRelease);

    m_buttonMatrixHead->show();
    pMode->start();
}

void GraphSceneView::writeChangesToGraph() {
    auto objects = getAllObjects();

    std::list<ObjectViewItems::ItemBase*> vertices;
    std::list<ObjectViewItems::ItemBase*> connections;

    LOG_INFO("Analysing objects on scene...");
    for (auto pObject : objects) {
        auto pCastedObject = dynamic_cast<ObjectViewItems::ItemBase*>(pObject);
        if (nullptr == pCastedObject) {
            continue;
        }

        if (pCastedObject->getType() ==
            ObjectViewConstants::OBJECTTYPE_VERTEX) {
            vertices.push_back(pCastedObject);
            continue;
        }

        if (pCastedObject->getType() ==
            ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION) {
            connections.push_back(pCastedObject);
        }
    }
    LOG_OK("Found", vertices.size(), "vertices and", connections.size(),
           "connections");

    auto pGraph = m_pGraphMaintaner->getExtendedObject();

    pGraph->clearVertices();
    LOG_INFO("Loading vertices from scene...");
    Graph::GVertex tmpVertex;
    for (auto vert : vertices) {
        auto vertCasted = static_cast<ObjectViewItems::VertexObject*>(vert);

        tmpVertex.id = vertCasted->getObjectId();
        tmpVertex.posX = vertCasted->x();
        tmpVertex.posY = vertCasted->y();

        tmpVertex.name = vertCasted->getName();
        tmpVertex.shortName = vertCasted->getShortName();
        tmpVertex.description = vertCasted->getDescription();

        tmpVertex.borderColor = vertCasted->getMainColor();
        tmpVertex.backgroundColor = vertCasted->getBackgroundColor();

        tmpVertex.image = vertCasted->getImage();

        tmpVertex.customProperties = vertCasted->getCustomProperties();

        pGraph->addVertex(tmpVertex);
    }
    LOG_OK("Loaded", pGraph->getVerticesCount(), "vertices from scene");

    pGraph->clearConnections();
    LOG_INFO("Loading connections from scene...");
    Graph::GConnection tmpConnection;
    for (auto con : connections) {
        auto conCasted =
            static_cast<ObjectViewItems::VertexConnectionLine*>(con);

        // Игнорируем невалидные соединения (например, которые в состоянии
        // редактирования)
        if (conCasted->getVertexFrom() == nullptr ||
            conCasted->getVertexTo() == nullptr) {
            LOG_WARNING("Skipped invalid connection:", conCasted->getName());
            continue;
        }

        tmpConnection.idFrom = conCasted->getVertexFrom()->getObjectId();
        tmpConnection.idTo = conCasted->getVertexTo()->getObjectId();

        tmpConnection.name = conCasted->getShortName();
        tmpConnection.lineColor = conCasted->getMainColor();

        tmpConnection.customProperties = conCasted->getCustomProperties();

        pGraph->addConnection(tmpConnection);
    }
    LOG_OK("Loaded", pGraph->getConnectionsCount(), "connections from scene");
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

    auto pGraph = m_pGraphMaintaner->getExtendedObject();

    auto vertices = pGraph->getAllVertices();
    std::unordered_map<GraphCommon::graphId_t, ObjectViewItems::VertexObject*>
        vertexObjects;

    for (auto& vert : vertices) {
        auto pVertexItem = createVertex(vert.id);

        if (!vert.image.isNull()) {
            pVertexItem->setImage(vert.image);
        }

        pVertexItem->setShortName(vert.shortName);
        pVertexItem->setName(vert.name);
        pVertexItem->setDescription(vert.description);
        pVertexItem->setCustomProperties(vert.customProperties);

        pVertexItem->setPos(vert.posX, vert.posY);
        pVertexItem->setRect(vertexRect);
        pVertexItem->setZValue(sceneConfig.vertexLayer);

        pVertexItem->setMainColor(vert.borderColor);
        pVertexItem->setBackgroundColor(vert.backgroundColor);

        vertexObjects[vert.id] = pVertexItem;
    }

    const GVertex* pConnectionFrom{nullptr};
    const GVertex* pConnectionTo{nullptr};

    QHash<GraphCommon::graphId_t, std::vector<GConnection> > connectionHash;

    for (auto& con : pGraph->getAllConnections()) {
        auto pConFrom = vertexObjects.find(con.idFrom);
        if (pConFrom == vertexObjects.end()) {
            throw std::runtime_error("Vertex from did not found!");
        }

        auto pConTo = vertexObjects.find(con.idTo);
        if (pConTo == vertexObjects.end()) {
            throw std::runtime_error("Target vertex did not found!");
        }

        auto pConnection = createConnectionLine(con.idFrom, con.idTo);

        pConnection->setMainColor(con.lineColor);
        pConnection->setZValue(sceneConfig.connectionLineLayer);

        pConnection->setShortName(con.name);

        pConFrom->second->subscribeAsConnectionFrom(pConnection);
        pConTo->second->subscribeAsConnectionTo(pConnection);
    }
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

    auto pVertexItem = new ObjectViewItems::VertexObject;
    pVertexItem->setObjectId(m_currentItemId);

    pVertexItem->setShortName("My node");
    pVertexItem->setName("My node template");
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

ObjectViewItems::VertexObject* GraphSceneView::createVertex(
    ObjectViewConstants::objectId_t vId) {
    if (!isIdAvailable(vId)) {
        LOG_ERROR("Got unavailable id:", vId);
        return nullptr;
    }

    auto pVertexItem = new ObjectViewItems::VertexObject;
    pVertexItem->setObjectId(vId);

    pVertexItem->setShortName("My node");
    pVertexItem->setName("My node template");
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
