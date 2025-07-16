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

        m_buttonMatrixHead->setButtonsSize(QSize(35, 35));
        m_buttonMatrixHead->setButtonMatrix(-10, -10, 0, 0);
        m_buttonMatrixHead->setButtonMargin(10);

        m_buttonMatrixHead->setAnimationSpeed(1.5);
        m_buttonMatrixHead->setIcons(QIcon("://DATA/images/icons/tools_open.svg"), QIcon("://DATA/images/icons/tools_close.svg"));
        m_buttonMatrixHead->setButtonPadding(0, 30, 0, 30);
        m_buttonMatrixHead->collapse(false);
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
    pMode->setGraphScene(this);

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
        pVertexItem->setToolTip(vert.name);

        addObject(pVertexItem);
    }

    const GVertex* pConnectionFrom {nullptr};
    const GVertex* pConnectionTo {nullptr};

    QHash<uint, std::vector<GConnection> > connectionHash;

    for (auto& con : m_pGraph->getAllConnections()) {
        pConnectionFrom = nullptr;
        pConnectionTo = nullptr;

        for (auto& vert : vertices) {
            if (vert.id == con.idFrom) {
                pConnectionFrom = &vert;
            }

            if (vert.id == con.idTo) {
                pConnectionTo = &vert;
            }

            if (pConnectionFrom != nullptr && pConnectionTo != nullptr) {
                break;
            }
        }

        if (pConnectionFrom == nullptr || pConnectionTo == nullptr) {
            throw std::runtime_error("One of vertices did not found!");
        }

        auto connectionCountIt = connectionHash.find(con.idTo);
        if (connectionCountIt == connectionHash.end()) {
            connectionHash.insert(con.idTo, m_pGraph->getConnectionsToVertex(con.idTo));
            connectionCountIt = connectionHash.find(con.idTo);
        }

        uint connectionCount = connectionCountIt.value().size() + 1;
        uint connectionNumber = 1;

        for (const auto& countCon : connectionCountIt.value()) {
            if (con == countCon) {
                break;
            }
            connectionNumber++;
        }

        auto pConnection = new PredefinedObjects::VertexConnectionLine;

        auto isConnectionFromLeft = pConnectionFrom->posX < pConnectionTo->posX;
        double connectionOffsetMultiplier = (isConnectionFromLeft ? -1 : 1);

        auto lineOffset = static_cast<double>(connectionNumber) / (static_cast<double>(connectionCount));
        auto xOffset =
                (isConnectionFromLeft ? 0 : vertexRadius) +
                lineOffset * vertexRadius -
                connectionOffsetMultiplier * pConnection->getArrowSize();

        auto fromPos = QPointF(pConnectionFrom->posX + vertexRadius, pConnectionFrom->posY + 2 * vertexRadius + pConnection->getArrowSize());
        auto toPos = QPointF(pConnectionTo->posX + xOffset, pConnectionTo->posY - pConnection->getArrowSize());

        pConnection->setLine(QLineF(fromPos, toPos));

        pConnection->setPen(con.lineColor);
        pConnection->setZValue(conversionConfig.connectionLineLayer);

        pConnection->setData(ObjectSceneConstants::OBJECTFIELD_NAME, con.name);
        pConnection->setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE,      OBJECT_TYPE_CONNECTION);
        pConnection->setToolTip(con.name);
        addObject(pConnection);
    }
}


}
