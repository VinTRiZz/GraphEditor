#include "graphscenebase.h"

#include "GUI/ObjectScene/predefinedobjects.h"

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
        m_buttonMatrixHead->setButtonMatrix(-10, -10, 0, 0);
        m_buttonMatrixHead->setButtonMargin(10);

        m_buttonMatrixHead->setAnimationSpeed(0.5);
        m_buttonMatrixHead->setIcons(QIcon("://DATA/images/icons/tools_open.svg"), QIcon("://DATA/images/icons/tools_close.svg"));
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

    // TODO: Вместо удаления, обновить
    removeAllObjects();

    auto& sceneConfig = GraphSceneConfiguration::getInstance();
    double labelHeight {0};

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);

    auto vertices = m_pGraph->getAllVertices();
    std::unordered_map<uint, PredefinedObjects::VertexObject*> vertexObjects;

    for (auto& vert : vertices) {
        auto pVertexItem = new PredefinedObjects::VertexObject;

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

        addObject(pVertexItem);
        vertexObjects[vert.id] = pVertexItem;
    }

    const GVertex* pConnectionFrom {nullptr};
    const GVertex* pConnectionTo {nullptr};

    QHash<uint, std::vector<GConnection> > connectionHash;

    for (auto& con : m_pGraph->getAllConnections()) {
        auto pConFrom = vertexObjects.find(con.idFrom);
        if (pConFrom == vertexObjects.end()) {
            throw std::runtime_error("Vertex from did not found!");
        }

        auto pConTo   = vertexObjects.find(con.idTo);
        if (pConTo == vertexObjects.end()) {
            throw std::runtime_error("Target vertex did not found!");
        }

        auto pConnection = new PredefinedObjects::VertexConnectionLine;

        pConnection->setPen(con.lineColor);
        pConnection->setZValue(sceneConfig.connectionLineLayer);

        pConnection->setName(con.name);
        pConnection->setToolTip(con.name);
        addObject(pConnection);

        pConFrom->second->subscribeAsConnectionFrom(pConnection);
        pConTo->second->subscribeAsConnectionTo(pConnection);
    }
}

PredefinedObjects::VertexConnectionLine *GraphSceneBase::createConnectionLine()
{
    auto pConnection = new PredefinedObjects::VertexConnectionLine;

    pConnection->setZValue(GraphSceneConfiguration::getInstance().connectionLineLayer);

    pConnection->setName({});
    addObject(pConnection);

    return pConnection;
}

PredefinedObjects::VertexObject *GraphSceneBase::createVertex()
{
    auto pVertexItem = new PredefinedObjects::VertexObject;

    pVertexItem->setShortName("My node");
    pVertexItem->setName("My node template");
    pVertexItem->setDescription("My example description");

    auto& sceneConfig = GraphSceneConfiguration::getInstance();
    pVertexItem->setZValue(sceneConfig.vertexLayer);

    QRect vertexRect;
    vertexRect.setWidth(sceneConfig.vertexWidth);
    vertexRect.setHeight(sceneConfig.vertexWidth);
    pVertexItem->setRect(vertexRect);

    addObject(pVertexItem);

    return pVertexItem;
}


}
