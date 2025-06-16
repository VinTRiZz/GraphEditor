#include "graphdrawer.h"

#include "objectsceneconstants.h"

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

    auto pResItem = new QGraphicsRectItem;

    // TODO: Вычислять размеры графа или менять в динамике
    QRect resRect;
    resRect.setWidth(500);
    resRect.setHeight(500);
    pResItem->setRect(resRect);

    QRect vertexRect;
    vertexRect.setWidth(100);
    vertexRect.setHeight(80);

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
            auto scaledPxmap = vert.pxmap.scaled(QSize(50, 50));
            static_cast<QGraphicsPixmapItem*>(pVertexItem)->setPixmap(scaledPxmap);
        }
        pVertexItem->setParentItem(pResItem);

        auto pVertexLabel = new QGraphicsSimpleTextItem;
        pVertexLabel->setParentItem(pVertexItem);
        pVertexLabel->setPen(vert.borderColor);
        pVertexLabel->setBrush(Qt::white);
        pVertexLabel->setText(vert.shortName);

        // TODO: Найти способ задать центральной позицию получше
        pVertexLabel->setX(vertexRect.center().x() / 2 - pVertexLabel->font().pixelSize() * vert.shortName.size() / 2);
        pVertexLabel->setY(vertexRect.center().y() - 10);
        pVertexLabel->setZValue(conversionConfig.vertexDataLayer);

        pVertexItem->setX(vert.posX);
        pVertexItem->setY(vert.posY);
        pVertexItem->setZValue(conversionConfig.vertexLayer);
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

        auto pConnection = new QGraphicsLineItem;

        QLineF connectionLine;
        connectionLine.setP1(QPoint(pConnectionFrom->posX + vertexRect.width() / 2, pConnectionFrom->posY + vertexRect.height() / 2));
        connectionLine.setP2(QPoint(pConnectionTo->posX + vertexRect.width() / 2, pConnectionTo->posY + vertexRect.height() / 2));
        pConnection->setLine(connectionLine);
        pConnection->setPen(con.lineColor);
        pConnection->setZValue(conversionConfig.connectionLineLayer);
        pConnection->setParentItem(pResItem);

        auto pConnectionLabel = new QGraphicsSimpleTextItem;
        pConnectionLabel->setParentItem(pConnection);

        pConnectionLabel->setPen(con.lineColor);
        pConnectionLabel->setText(con.name);

        auto rotationAngle = -connectionLine.angle();

        LOG_DEBUG_SYNC("Connection:", con.name, "Angle:", rotationAngle);

//        pConnectionLabel->setRotation(rotationAngle);

        // TODO: Найти способ задать центральной позицию получше
        auto textCos = cos(rotationAngle * M_PI / 180.0);
        auto textSin = sin(rotationAngle * M_PI / 180.0);
        auto textWidth = pConnectionLabel->font().pixelSize() * con.name.size();
        pConnectionLabel->setX(connectionLine.center().x() + textWidth * textSin);
        pConnectionLabel->setY(connectionLine.center().y() + textWidth * textCos);
        pConnectionLabel->setZValue(conversionConfig.connectionTextLayer);

        auto pConnectionLabelRect = new QGraphicsRectItem;
        pConnectionLabelRect->setParentItem(pConnection);
        pConnectionLabelRect->setBrush(Qt::white);

        auto labelRect = pConnectionLabel->boundingRect();
        labelRect.setX(labelRect.x() - 10 * textCos);
        labelRect.setY(labelRect.y() - 10 * textSin);
        labelRect.setWidth(labelRect.width() + 10);
        pConnectionLabelRect->setRect(labelRect);
        pConnectionLabelRect->setX(pConnectionLabel->x());
        pConnectionLabelRect->setY(pConnectionLabel->y());
        pConnectionLabelRect->setZValue(conversionConfig.connectionRectLayer);
    }

    m_pScene->addObject(pResItem);
}

void GraphDrawer::startEditMode()
{
    m_currentMode = CurrentDrawerMode::Edit;
}

void GraphDrawer::startViewMode()
{
    m_currentMode = CurrentDrawerMode::View;
}

void GraphDrawer::stopMode()
{
    m_currentMode = CurrentDrawerMode::None;
}
