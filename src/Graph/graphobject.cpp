#include "graphobject.h"

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
    return nullptr;
}

uint GraphObject::addVertex(const GVertex &iVert)
{
    m_vertices.push_back(iVert);
    uint resId = m_vertices.back().id = m_idGenerator();
    return resId;
}

bool GraphObject::updateVertex(const GVertex &iVert)
{
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
