#include "graphobject.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>

#include <QBrush>
#include <QPen>

#include "logging.h"

namespace Graph
{


GraphObject::GraphObject()
{
    uint currentId {0};
    m_idGenerator = [currentId]() mutable {
        return currentId++;
    };
}

void GraphObject::setIdGenerator(const std::function<uint ()> &fGen)
{
    if (!fGen) {
        throw std::invalid_argument("ObjectScene: invalid id generator passed into setIdGenerator");
    }

    m_idGenerator = fGen;
}

QGraphicsItem *GraphObject::toItem() const
{
    auto pResItem = new QGraphicsRectItem;

    // TODO: Вычислять размеры графа или менять в динамике
    QRect resRect;
    resRect.setWidth(500);
    resRect.setHeight(500);
    pResItem->setRect(resRect);

    QRect vertexRect;
    vertexRect.setWidth(100);
    vertexRect.setHeight(80);

    for (auto& vert : m_vertices) {
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
        pVertexItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true); // Чтобы размер не менялся от скейла

        auto pVertexLabel = new QGraphicsTextItem;
        pVertexLabel->setParentItem(pVertexItem);
        pVertexLabel->setDefaultTextColor(vert.borderColor);
        pVertexLabel->setPlainText(vert.shortName);

        // TODO: Найти способ задать центральной позицию получше
        pVertexLabel->setX(vertexRect.center().x() / 2 - pVertexLabel->textWidth() * vert.shortName.size() / 2);
        pVertexLabel->setY(vertexRect.center().y() - 10);
        pVertexLabel->setZValue(m_vertexDataLayer);

        if (vert.posX == 0 && vert.posY == 0) {
            // TODO: Вычислить положение вершины
        }

        pVertexItem->setX(vert.posX);
        pVertexItem->setY(vert.posY);
        pVertexItem->setZValue(m_vertexLayer);
    }

    const GVertex* pConnectionFrom {nullptr};
    const GVertex* pConnectionTo {nullptr};
    for (auto& con : m_connections) {
        pConnectionFrom = nullptr;
        pConnectionTo = nullptr;

        for (auto& vert : m_vertices) {
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
        pConnection->setZValue(m_connectionLineLayer);
        pConnection->setParentItem(pResItem);

        auto pConnectionLabel = new QGraphicsTextItem;
        pConnectionLabel->setParentItem(pConnection);
        pConnectionLabel->setDefaultTextColor(con.lineColor);
        pConnectionLabel->setPlainText(con.name);

        pConnectionLabel->setRotation(connectionLine.angle());

        // TODO: Найти способ задать центральной позицию получше
        pConnectionLabel->setX(connectionLine.center().x() + pConnectionLabel->textWidth() * con.name.size() * cos(connectionLine.angle() * M_PI / 180.0));
        pConnectionLabel->setY(connectionLine.center().y() + pConnectionLabel->textWidth() * con.name.size() * sin(connectionLine.angle() * M_PI / 180.0));
        pConnectionLabel->setZValue(m_vertexDataLayer);
    }

    return pResItem;
}

uint GraphObject::addVertex(const GVertex &iVert)
{
    if (!iVert.isShortnameValid()) {
        throw std::invalid_argument("GraphObject::addVertex: invalid size of short name");
    }
    m_vertices.push_back(iVert);
    uint resId = m_vertices.back().id = m_idGenerator();
    return resId;
}

bool GraphObject::updateVertex(const GVertex &iVert)
{
    if (!iVert.isValid()) {
        throw std::invalid_argument("GraphObject::updateVertex: invalid size of short name");
    }
    auto targetVertex = std::find_if(m_vertices.begin(), m_vertices.end(), [&](auto& vert){
        return (vert.id == iVert.id);
    });
    if (targetVertex == m_vertices.end()) {
        return false;
    }
    *targetVertex = iVert;
    return true;
}

std::vector<GVertex> GraphObject::getAllVertices() const
{
    return m_vertices;
}

void GraphObject::removeVertex(uint vertexId)
{
    auto targetVertex = std::find_if(m_vertices.begin(), m_vertices.end(), [&](auto& vert){
        return (vert.id == vertexId);
    });
    if (targetVertex == m_vertices.end()) {
        return;
    }
    m_vertices.erase(targetVertex);
}

bool GraphObject::addConnection(const GConnection &iCon)
{
    if (!iCon.isValid()) {
        throw std::invalid_argument("GraphObject::addConnection: Invalid connection");
    }

    bool containIdTo {false};
    bool containIdFrom {false};

    for (auto& vert : m_vertices) {
        containIdFrom |= vert.id == iCon.idFrom;
        containIdTo |= vert.id == iCon.idTo;

        if (containIdFrom && containIdTo) {
            break;
        }
    }

    if (!containIdFrom || !containIdTo) {
        return false;
    }

    m_connections.push_back(iCon);
    return true;
}

std::vector<GConnection> GraphObject::getAllConnections() const
{
    return m_connections;
}

void GraphObject::removeConnection(uint conFrom, uint conTo)
{
    auto targetConnection = std::find_if(m_connections.begin(), m_connections.end(), [&](auto& con){
        return (con.idFrom == conFrom) && (con.idTo == conTo);
    });
    if (targetConnection == m_connections.end()) {
        return;
    }
    m_connections.erase(targetConnection);
}

void GraphObject::setName(const QString &iName)
{
    m_name = iName;
}

QString GraphObject::getName() const
{
    return m_name;
}

void GraphObject::setDescription(const QString &iDescr)
{
    m_description = iDescr;
}

QString GraphObject::getDescription() const
{
    return m_description;
}

void GraphObject::setCreateTime(const QDateTime &iDateTime)
{
    m_createTime = iDateTime;
}

QDateTime GraphObject::getCreateTime() const
{
    return m_createTime;
}

void GraphObject::setEditTime(const QDateTime &iDateTime)
{
    m_editTime = iDateTime;
}

QDateTime GraphObject::getEditTime() const
{
    return m_editTime;
}


}
