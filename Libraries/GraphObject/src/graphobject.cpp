#include "graphobject.h"
#include "graphcommon.h"

#include <QJsonObject>
#include <QDateTime>

#include <Common/Logging.h>

namespace Graph
{

GraphObject::GraphObject()
{

}

bool GraphObject::operator ==(const GraphObject &gObj_) const
{
    if (m_vertices != gObj_.m_vertices) {
        LOG_INFO("GraphObject::equal vertices are different");
        return false;
    }

    if (m_connections != gObj_.m_connections) {
        LOG_INFO("GraphObject::equal connections are different");
        return false;
    }
    return true;
}

bool GraphObject::operator !=(const GraphObject &gObj_) const
{
    return !(*this == gObj_);
}

bool GraphObject::addVertex(const GVertex &iVert)
{
    if (!iVert.isShortnameValid()) {
        throw std::invalid_argument("GraphObject::addVertex: invalid size of short name");
    }

    for (auto& vert : m_vertices) {
        if (vert.id == iVert.id) {
            return false;
        }
    }

    m_vertices.push_back(iVert);
    return true;
}

bool GraphObject::updateVertex(const GVertex &iVert)
{
    if (!iVert.isValid()) {
        throw std::invalid_argument("GraphObject::updateVertex: invalid size of short name");
    }
    auto targetVertex = std::find_if(m_vertices.begin(), m_vertices.end(), [&](const auto& vert){
        return (vert.id == iVert.id);
    });
    if (targetVertex == m_vertices.end()) {
        return false;
    }
    *targetVertex = iVert;
    return true;
}

std::optional<GVertex> GraphObject::getVertex(GraphCommon::graphId_t vertexId) const
{
    auto targetVertex = std::find_if(m_vertices.begin(), m_vertices.end(), [&](const auto& vert){
        return (vert.id == vertexId);
    });
    if (targetVertex == m_vertices.end()) {
        return std::nullopt;
    }
    return *targetVertex;
}

std::list<GVertex> GraphObject::getAllVertices() const
{
    return m_vertices;
}

std::size_t GraphObject::getVerticesCount() const
{
    return m_vertices.size();
}

void GraphObject::removeVertex(GraphCommon::graphId_t vertexId)
{
    auto targetVertex = std::find_if(m_vertices.begin(), m_vertices.end(), [&](const auto& vert){
        return (vert.id == vertexId);
    });
    if (targetVertex == m_vertices.end()) {
        return;
    }
    m_vertices.erase(targetVertex);
    removeConnections(vertexId);
}

void GraphObject::clearVertices()
{
    for (auto& vert : m_vertices) {
        removeConnections(vert.id);
    }
    m_vertices.clear();
}

bool GraphObject::addConnection(const GConnection &iCon)
{
    if (!iCon.isValid()) {
        return false;
    }

    bool containIdTo {false};
    bool containIdFrom {false};

    for (const auto& vert : m_vertices) {
        containIdFrom |= (vert.id == iCon.idFrom);
        containIdTo |= (vert.id == iCon.idTo);

        if (containIdFrom && containIdTo) {
            break;
        }
    }

    if (!containIdFrom || !containIdTo) {
        return false;
    }

    auto existingConnection = m_connections.equal_range(iCon.idFrom);
    if (std::any_of(existingConnection.first, existingConnection.second,
                    [&iCon](const auto& con){
        return con.second.idTo == iCon.idTo;
    })) {
        return false;
    }

    m_connections.emplace(iCon.idFrom, iCon);
    return true;
}

std::vector<GConnection> GraphObject::getConnectionsFromVertex(GraphCommon::graphId_t vertexId) const
{
    std::vector<GConnection> res;
    auto targetConnections = m_connections.equal_range(vertexId);
    for (auto con = targetConnections.first; con != targetConnections.second; ++con) {
        res.push_back(con->second);
    }
    return res;
}

std::optional<GConnection> GraphObject::getConnection(GraphCommon::graphId_t vertexFromId, GraphCommon::graphId_t vertexToId) const
{
    auto targetConRange = m_connections.equal_range(vertexFromId);

    // Not found
    if (targetConRange.first == targetConRange.second) {
        return std::nullopt;
    }

    while (targetConRange.first != targetConRange.second) {

        if (targetConRange.first->second.idTo == vertexToId) {
            return targetConRange.first->second;
        }
        targetConRange.first++;
    }
    return std::nullopt;
}

std::vector<GConnection> GraphObject::getAllConnections() const
{
    std::vector<GConnection> res;
    res.reserve(m_connections.size());
    for (const auto& [idFrom, connection] : m_connections) {
        res.push_back(connection);
    }
    return res;
}

std::size_t GraphObject::getConnectionsCount() const
{
    return m_connections.size();
}

void GraphObject::removeConnection(GraphCommon::graphId_t conFrom, GraphCommon::graphId_t conTo)
{
    auto targetConnections = m_connections.equal_range(conFrom);

    for (auto con = targetConnections.first; con != targetConnections.second; ++con) {
        if (con->second.idTo == conTo) {
            m_connections.erase(con);
            break;
        }
    }
}

void GraphObject::removeConnections(GraphCommon::graphId_t conFrom)
{
    m_connections.erase(conFrom);

    std::list<GraphCommon::graphId_t> connectedToTarget;
    for (auto& con : m_connections) {
        if (con.second.idFrom == conFrom) {
            connectedToTarget.push_back(con.first);
        }
    }

    for (auto& con : connectedToTarget) {
        auto range = m_connections.equal_range(conFrom);
        while (range.first->second.idTo != con) { range.first = std::next(range.first); }
        m_connections.erase(range.first);
    }
}

void GraphObject::clearConnections()
{
    m_connections.clear();
}

}
