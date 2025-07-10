#include "graphviewbase.h"

namespace Graph
{

struct GraphViewBase::Impl
{

};


GraphViewBase::GraphViewBase(QWidget *parent) :
    ObjectView(parent),
    d {std::make_unique<Impl>(Impl())}
{

}

GraphViewBase::~GraphViewBase()
{

}

void GraphViewBase::init()
{
    if (!isInited()) {
        ObjectView::init();
    }
}

void GraphViewBase::setCurrentGraph(std::shared_ptr<GraphObject> pGraph)
{
    m_pGraph = pGraph;
    updateGraph();
}

void GraphViewBase::updateGraph()
{
    if (!m_pGraph) {
        return;
    }

    // TODO: Вместо удаления, обновить
    clearScene();

    auto& conversionConfig = GraphConversionConfiguration::getInstance();

    const double vertexRadius = 50;
    double labelHeight {0};

    QRect vertexRect;
    vertexRect.setWidth(vertexRadius * 2);
    vertexRect.setHeight(vertexRadius * 2);

    auto vertices = m_pGraph->getAllVertices();

    for (auto& vert : vertices) {
        auto pVertexItem = new PredefinedObjects::VertexObject;

//        // TODO: Вернуть как решу вопрос с иконками
//        if (!vert.pxmap.isNull()) {
//            // Для отображения всего в унифицированном виде
//            auto scaledPxmap = vert.pxmap.scaled(QSize(vertexRadius * 2, vertexRadius * 2));
//            pVertexItem->setImage(scaledPxmap);
//        }

        pVertexItem->setText(vert.shortName);
        pVertexItem->setPos(vert.posX, vert.posY);
        pVertexItem->setRect(vertexRect);
        pVertexItem->setZValue(conversionConfig.vertexLayer);

        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT,      vert.shortName);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_NAME,            vert.name);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION,     vert.description);
        pVertexItem->setData(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON,   vert.customProperties);

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
            throw std::runtime_error("GraphObject::toItem: One of vertices did not found!");
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

        auto xOffset = (static_cast<double>(connectionNumber) * static_cast<double>(vertexRect.width()) / static_cast<double>(connectionCount)) - pConnection->getArrowSize();

        auto fromPos = QPointF(pConnectionFrom->posX + vertexRadius, pConnectionFrom->posY + 2 * vertexRadius + pConnection->getArrowSize());
        auto toPos = QPointF(pConnectionTo->posX + xOffset, pConnectionTo->posY - pConnection->getArrowSize());

        pConnection->setLine(QLineF(fromPos, toPos));

        pConnection->setPen(con.lineColor);
        pConnection->setZValue(conversionConfig.connectionLineLayer);

        pConnection->setData(ObjectSceneConstants::OBJECTFIELD_NAME, con.name);
        addObject(pConnection);
    }
}


}
