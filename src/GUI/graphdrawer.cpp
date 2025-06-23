#include "graphdrawer.h"

#include "objectsceneconstants.h"

#include "vertexconnectionline.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>

#include <QFont>
#include <QBrush>
#include <QPen>

#include "logging.h"

using namespace Graph;

GraphDrawer::GraphDrawer()
{

}

GraphDrawer::~GraphDrawer()
{

}

GraphDrawer::CurrentDrawerMode GraphDrawer::getCurrentMode() const
{
    return m_currentMode;
}

void GraphDrawer::setScene(ObjectScene *pScene)
{
    m_pScene = pScene;
}

void GraphDrawer::setCurrentGraph(Graph::GraphObject *pGraph)
{
    m_pGraph = pGraph;
}

void GraphDrawer::setOverlayButtonList(OverlayButtonList *pOverlayButton)
{
    m_pOverlayButton = pOverlayButton;
}

void GraphDrawer::updateGraph()
{
    if (m_pGraph == nullptr || m_pScene == nullptr || m_pOverlayButton == nullptr) {
        return;
    }

    // TODO: Вместо удаления, обновить
    m_pScene->clearScene();

    auto& conversionConfig = GraphConversionConfiguration::getInstance();

    const double vertexRadius = 50;
    double labelHeight {0};

    QRect vertexRect;
    vertexRect.setWidth(vertexRadius * 2);
    vertexRect.setHeight(vertexRadius * 2);

    auto vertices = m_pGraph->getAllVertices();

    for (auto& vert : vertices) {
        QGraphicsItem* pVertexItem {nullptr};

        if (vert.pxmap.isNull()) {
            pVertexItem = new QGraphicsRectItem;
            static_cast<QGraphicsRectItem*>(pVertexItem)->setBrush(vert.backgroundColor);
            static_cast<QGraphicsRectItem*>(pVertexItem)->setPen(vert.borderColor);
            static_cast<QGraphicsRectItem*>(pVertexItem)->setRect(vertexRect);
        } else {
            pVertexItem = new QGraphicsPixmapItem;

            // Для отображения всего в унифицированном виде
            auto scaledPxmap = vert.pxmap.scaled(QSize(vertexRadius * 2, vertexRadius * 2));
            static_cast<QGraphicsPixmapItem*>(pVertexItem)->setPixmap(scaledPxmap);
        }

        auto pVertexLabel = new QGraphicsTextItem;
        pVertexLabel->setParentItem(pVertexItem);
        pVertexLabel->setPlainText(vert.shortName);
        pVertexLabel->setDefaultTextColor(vert.borderColor);

        pVertexLabel->setX(vertexRect.center().x() / 2 - pVertexLabel->textWidth() / 2);
        pVertexLabel->setY(vertexRect.center().y() + vertexRadius);
        pVertexLabel->setZValue(conversionConfig.vertexDataLayer);

        auto pVertexContrastRect = new QGraphicsRectItem;
        pVertexContrastRect->setParentItem(pVertexItem);
        auto contrastRect = pVertexLabel->boundingRect();
        contrastRect.moveTo(pVertexLabel->x(), pVertexLabel->y());
        pVertexContrastRect->setRect(contrastRect);
        pVertexContrastRect->setPen(vert.borderColor);
        pVertexContrastRect->setBrush(Qt::white);
        pVertexContrastRect->setZValue(conversionConfig.vertexDataRectLayer);

        labelHeight = pVertexLabel->boundingRect().height();

        pVertexItem->setX(vert.posX - pVertexItem->boundingRect().width() / 2);
        pVertexItem->setY(vert.posY - pVertexItem->boundingRect().height() / 2);
        pVertexItem->setZValue(conversionConfig.vertexLayer);
        m_pScene->addObject(pVertexItem);
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

        for (auto& countCon : connectionCountIt.value()) {
            if (con == countCon) {
                break;
            }
            connectionNumber++;
        }

        auto pConnection = new PredefinedObjects::VertexConnectionLine;

        auto xOffset = (static_cast<double>(connectionNumber) * static_cast<double>(vertexRect.width()) / static_cast<double>(connectionCount)) - pConnection->arrowSize();

        auto fromPos = QPointF(pConnectionFrom->posX, pConnectionFrom->posY + vertexRadius + labelHeight + 2);
        auto toPos = QPointF(pConnectionTo->posX - vertexRadius + xOffset, pConnectionTo->posY - vertexRadius - 2);

        pConnection->setPositionFrom(fromPos);
        pConnection->setPositionTo(toPos);

        pConnection->setPen(QPen(con.lineColor, 3));
        pConnection->setZValue(conversionConfig.connectionLineLayer);
        m_pScene->addObject(pConnection);
    }
}

void GraphDrawer::startEditMode()
{
    m_currentMode = CurrentDrawerMode::Edit;

    LOG_DEBUG("Started edit mode");
}

void GraphDrawer::startViewMode()
{
    m_currentMode = CurrentDrawerMode::View;

    LOG_DEBUG("Started view mode");
}

void GraphDrawer::stopMode()
{
    m_currentMode = CurrentDrawerMode::None;

    LOG_DEBUG("Disabled mode");
}
