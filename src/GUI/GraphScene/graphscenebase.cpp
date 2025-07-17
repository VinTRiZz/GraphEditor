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

    auto& conversionConfig = GraphSceneConfiguration::getInstance();

    const double vertexRadius = 50;
    double labelHeight {0};

    QRect vertexRect;
    vertexRect.setWidth(vertexRadius * 2);
    vertexRect.setHeight(vertexRadius * 2);

    auto vertices = m_pGraph->getAllVertices();
    std::unordered_map<uint, PredefinedObjects::VertexObject*> vertexObjects;

    for (auto& vert : vertices) {
        auto pVertexItem = new PredefinedObjects::VertexObject;

        if (!vert.image.isNull()) {
            pVertexItem->setImage(vert.image);
        }

        pVertexItem->setText(vert.shortName);
        pVertexItem->setPos(vert.posX, vert.posY);
        pVertexItem->setRect(vertexRect);
        pVertexItem->setZValue(conversionConfig.vertexLayer);

        pVertexItem->setNodeColor(vert.borderColor, vert.backgroundColor);

        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT,      vert.shortName);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME,            vert.name);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION,     vert.description);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON,   vert.customProperties);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE,      OBJECT_TYPE_VERTEX);

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
        pConnection->setZValue(conversionConfig.connectionLineLayer);

        pConnection->setData(ObjectSceneConstants::OBJECTFIELD_NAME, con.name);
        pConnection->setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE, OBJECT_TYPE_CONNECTION);
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

    pConnection->setData(ObjectSceneConstants::OBJECTFIELD_NAME, {});
    pConnection->setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE, OBJECT_TYPE_CONNECTION);
    addObject(pConnection);

    return pConnection;
}

PredefinedObjects::VertexObject *GraphSceneBase::createVertex()
{
    auto pVertexItem = new PredefinedObjects::VertexObject;

    pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT,      "My node");
    pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME,            "My node template");
    pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION,     {});
    pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON,   {});
    pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE,      OBJECT_TYPE_VERTEX);

    addObject(pVertexItem);
    return pVertexItem;
}


}
