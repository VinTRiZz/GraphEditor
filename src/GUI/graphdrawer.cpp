#include "graphdrawer.h"

#include "objectsceneconstants.h"

#include "arrowline.h"

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

    QRect vertexRect;
    vertexRect.setWidth(vertexRadius * 2);
    vertexRect.setHeight(vertexRadius * 2);

    auto vertices = m_pGraph->getAllVertices();

    for (auto& vert : vertices) {
        QGraphicsItem* pVertexItem {nullptr};

        if (vert.pxmap.isNull()) {
            pVertexItem = new QGraphicsEllipseItem;
            static_cast<QGraphicsEllipseItem*>(pVertexItem)->setBrush(vert.backgroundColor);
            static_cast<QGraphicsEllipseItem*>(pVertexItem)->setPen(vert.borderColor);
            static_cast<QGraphicsEllipseItem*>(pVertexItem)->setRect(vertexRect);
        } else {
            pVertexItem = new QGraphicsPixmapItem;
            static_cast<QGraphicsEllipseItem*>(pVertexItem)->setPen(QPen(Qt::white, 1.5)); // Для контраста

            // Для отображения всего в унифицированном виде
            auto scaledPxmap = vert.pxmap.scaled(QSize(vertexRadius, vertexRadius));
            static_cast<QGraphicsPixmapItem*>(pVertexItem)->setPixmap(scaledPxmap);
        }

        auto pVertexLabel = new QGraphicsSimpleTextItem;
        pVertexLabel->setParentItem(pVertexItem);
        pVertexLabel->setPen(vert.borderColor);
        pVertexLabel->setBrush(Qt::white);
        pVertexLabel->setText(vert.shortName);

        // TODO: Найти способ задать центральной позицию получше
        pVertexLabel->setX(vertexRect.center().x() / 2 - pVertexLabel->font().pixelSize() * vert.shortName.size() / 2);
        pVertexLabel->setY(vertexRect.center().y() - 10);
        pVertexLabel->setZValue(conversionConfig.vertexDataLayer);

        pVertexItem->setX(vert.posX - vertexRadius);
        pVertexItem->setY(vert.posY - vertexRadius);
        pVertexItem->setZValue(conversionConfig.vertexLayer);
        m_pScene->addObject(pVertexItem);
    }

    const GVertex* pConnectionFrom {nullptr};
    const GVertex* pConnectionTo {nullptr};
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

        auto pConnection = new PredefinedObjects::ArrowedLine;

        QLineF connectionLine;
        connectionLine.setP1(QPoint(pConnectionFrom->posX, pConnectionFrom->posY));
        connectionLine.setP2(QPoint(pConnectionTo->posX, pConnectionTo->posY));

        auto rotationAngle = connectionLine.angle();

        auto vertexCircleCos = cos(rotationAngle * M_PI / 180.0);
        auto vertexCircleSin = -sin(rotationAngle * M_PI / 180.0);

        auto p1 = connectionLine.p1();
        auto p2 = connectionLine.p2();

        connectionLine.setP1(QPoint(p1.x() + vertexCircleCos * vertexRadius, p1.x() + vertexCircleSin * vertexRadius));

        if (p1.x() < p2.x()) {
            vertexCircleCos *= -1;
        }

        if (p1.y() < p2.y()) {
            vertexCircleSin *= -1;
        }

        connectionLine.setP2(QPoint(p2.x() + vertexCircleCos * vertexRadius, p2.x() + vertexCircleSin * vertexRadius));

        pConnection->setLine(connectionLine);
        pConnection->setPen(QPen(con.lineColor, 3));
        pConnection->setZValue(conversionConfig.connectionLineLayer + 30);
        m_pScene->addObject(pConnection);

        LOG_DEBUG_SYNC("Connection:", con.name, "Angle:", rotationAngle);

//        auto pConnectionLabel = new QGraphicsSimpleTextItem;
//        pConnectionLabel->setParentItem(pConnection);

//        pConnectionLabel->setPen(con.lineColor);
//        pConnectionLabel->setText(con.name);

//        pConnectionLabel->setRotation(rotationAngle);

//        auto textWidth = pConnectionLabel->font().pixelSize() * con.name.size();
//        pConnectionLabel->setX(connectionLine.center().x() + textWidth * textSin);
//        pConnectionLabel->setY(connectionLine.center().y() + textWidth * textCos);
//        pConnectionLabel->setZValue(conversionConfig.connectionTextLayer);

//        auto pConnectionLabelRect = new QGraphicsRectItem;
//        pConnectionLabelRect->setParentItem(pConnection);
//        pConnectionLabelRect->setBrush(Qt::white);

//        auto labelRect = pConnectionLabel->boundingRect();
//        labelRect.setX(labelRect.x() - 10 * textCos);
//        labelRect.setY(labelRect.y() - 10 * textSin);
//        labelRect.setWidth(labelRect.width() + 10);
//        pConnectionLabelRect->setRect(labelRect);
//        pConnectionLabelRect->setX(pConnectionLabel->x());
//        pConnectionLabelRect->setY(pConnectionLabel->y());
//        pConnectionLabelRect->setZValue(conversionConfig.connectionRectLayer);
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
