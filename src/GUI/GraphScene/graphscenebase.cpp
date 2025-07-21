#include "graphscenebase.h"

#include "Graph/graphcommon.h"
#include "GUI/ObjectScene/objectsceneconstants.h"
#include "logging.h"

namespace Graph
{

GraphSceneBase::GraphSceneBase(QWidget *parent) :
    ObjectView(parent)
{

}

GraphSceneBase::~GraphSceneBase()
{

}

void GraphSceneBase::init()
{
    if (!isInited()) {
        ObjectView::init();

        m_buttonMatrixHead = new ButtonMatrix::HeadButton(this);

        m_buttonMatrixHead->setButtonsSize(QSize(50, 50));
        m_buttonMatrixHead->setButtonMatrix(-10, 0, 0, 10);
        m_buttonMatrixHead->setButtonMargin(10);

        m_buttonMatrixHead->setAnimationSpeed(0.5);
        m_buttonMatrixHead->setIcons(QIcon("://DATA/images/icons/common/tools_open.svg"), QIcon("://DATA/images/icons/common/tools_close.svg"));
        m_buttonMatrixHead->setButtonPadding(0, 30, 0, 30);
        m_buttonMatrixHead->collapse(false);
        m_buttonMatrixHead->hide();
    }
}

void GraphSceneBase::setMode(GraphModeBase *pMode)
{
    if (nullptr == pMode) [[unlikely]] {
        throw std::invalid_argument("Invalid mode set (nullptr)");
    }

    if (nullptr != m_pCurrentMode) {
        disconnect(m_pCurrentMode, nullptr, this, nullptr);
        disconnect(this, nullptr, m_pCurrentMode, nullptr);
        m_pCurrentMode->stop();
    }

    m_pCurrentMode = pMode;

    connect(this, &ObjectView::pressedOnItem,
            m_pCurrentMode, &GraphModeBase::processPress);
    connect(this, &ObjectView::releasedOnItem,
            m_pCurrentMode, &GraphModeBase::processRelease);

    m_buttonMatrixHead->show();
    pMode->start();
}

void GraphSceneBase::writeChangesToGraph()
{
    auto objects = getAllObjects();

    std::list<ObjectViewItems::ItemBase*> vertices;
    std::list<ObjectViewItems::ItemBase*> connections;

    LOG_INFO("Analysing objects on scene...");
    for (auto pObject : objects) {
        auto pCastedObject = dynamic_cast<ObjectViewItems::ItemBase*>(pObject);
        if (nullptr == pCastedObject) {
            continue;
        }

        if (pCastedObject->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX) {
            vertices.push_back(pCastedObject);
            continue;
        }

        if (pCastedObject->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION) {
            connections.push_back(pCastedObject);
        }
    }
    LOG_OK("Found", vertices.size(), "vertices and", connections.size(), "connections");

    auto pGraph = getCurrentGraph();
    *pGraph = Graph::GraphObject();

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

    LOG_INFO("Loading connections from scene...");
    Graph::GConnection tmpConnection;
    for (auto con : connections) {
        auto conCasted = static_cast<ObjectViewItems::VertexConnectionLine*>(con);

        // Игнорируем невалидные соединения (например, которые в состоянии редактирования)
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

void GraphSceneBase::setCurrentGraph(Graph::GraphExtendedObject* pGraph)
{
    m_pGraph = pGraph;
    updateGraph();
}

GraphExtendedObject *GraphSceneBase::getCurrentGraph() const
{
    return m_pGraph;
}

ButtonMatrix::HeadButton *GraphSceneBase::getButtonMatrixHead() const
{
    return m_buttonMatrixHead;
}

void GraphSceneBase::updateGraph()
{
    if (!m_pGraph) {
        return;
    }

    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_VERTEX);
    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_VERTEX_CONNECTION);
    removeSpecialObjects(ObjectViewConstants::OBJECTTYPE_ARROWLINE);

    auto& sceneConfig = ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    double labelHeight {0};

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);

    auto vertices = m_pGraph->getAllVertices();
    std::unordered_map<GraphCommon::graphId_t, ObjectViewItems::VertexObject*> vertexObjects;

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

        pVertexItem->setNodeColor(vert.borderColor, vert.backgroundColor);

        vertexObjects[vert.id] = pVertexItem;
    }

    const GVertex* pConnectionFrom {nullptr};
    const GVertex* pConnectionTo {nullptr};

    QHash<GraphCommon::graphId_t, std::vector<GConnection> > connectionHash;

    for (auto& con : m_pGraph->getAllConnections()) {
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
        pConnection->setToolTip(con.name);

        pConFrom->second->subscribeAsConnectionFrom(pConnection);
        pConTo->second->subscribeAsConnectionTo(pConnection);
    }
}

ObjectViewItems::VertexConnectionLine *GraphSceneBase::createConnectionLine(ObjectViewConstants::objectId_t idFrom, ObjectViewConstants::objectId_t idTo)
{
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }

    auto pConnection = new ObjectViewItems::VertexConnectionLine;
    pConnection->setObjectId(m_currentItemId);
    pConnection->setZValue(ObjectViewConstants::ObjectSceneConfiguration::getInstance().connectionLineLayer);
    addObject(pConnection);

    return pConnection;
}

ObjectViewItems::VertexObject *GraphSceneBase::createVertex()
{
    while (!isIdAvailable(m_currentItemId)) {
        m_currentItemId++;
    }

    auto pVertexItem = new ObjectViewItems::VertexObject;
    pVertexItem->setObjectId(m_currentItemId);

    pVertexItem->setShortName("My node");
    pVertexItem->setName("My node template");
    pVertexItem->setDescription("My example description");

    auto& sceneConfig = ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    pVertexItem->setZValue(sceneConfig.vertexLayer);

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);
    pVertexItem->setRect(vertexRect);

    addObject(pVertexItem);
    return pVertexItem;
}

ObjectViewItems::VertexObject *GraphSceneBase::createVertex(ObjectViewConstants::objectId_t vId)
{
    if (!isIdAvailable(vId)) {
        LOG_ERROR("Got unavailable id:", vId);
        return nullptr;
    }

    auto pVertexItem = new ObjectViewItems::VertexObject;
    pVertexItem->setObjectId(vId);

    pVertexItem->setShortName("My node");
    pVertexItem->setName("My node template");
    pVertexItem->setDescription("My example description");

    auto& sceneConfig = ObjectViewConstants::ObjectSceneConfiguration::getInstance();
    pVertexItem->setZValue(sceneConfig.vertexLayer);

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);
    pVertexItem->setRect(vertexRect);

    addObject(pVertexItem);
    return pVertexItem;
}

void GraphSceneBase::resizeEvent(QResizeEvent *e)
{
    m_buttonMatrixHead->fixPositions();
    ObjectView::resizeEvent(e);
}


}
