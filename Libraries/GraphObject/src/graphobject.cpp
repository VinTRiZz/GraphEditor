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
    if (!std::equal(m_vertices.begin(), m_vertices.end(),
                              gObj_.m_vertices.begin(), gObj_.m_vertices.end())) {
        return false;
    }

    if (!std::equal(m_connections.begin(), m_connections.end(),
                              gObj_.m_connections.begin(), gObj_.m_connections.end())) {
        return false;
    }

    for (const auto& customProp : m_customDataValues) {
        auto gObjValue = gObj_.m_customDataValues.find(customProp.first);
        if (gObjValue == gObj_.m_customDataValues.end()) {
            return false;
        }

        if (customProp.second != gObjValue->second) {
            return false;
        }
    }

    if ((m_name         != gObj_.m_name) ||
        (m_description  != gObj_.m_description) ||
        (m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT) != gObj_.m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT)) ||
        (m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT)   != gObj_.m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT))
        ) {
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
        throw std::invalid_argument("GraphObject::addConnection: Invalid connection");
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

void GraphObject::setCustomValue(const QString &key, const QVariant &value)
{
    m_customDataValues[key] = value;
}

void GraphObject::removeCustomValue(const QString &key)
{
    auto targetValIt = m_customDataValues.find(key);
    if (targetValIt == m_customDataValues.end()) {
        return;
    }
    m_customDataValues.erase(targetValIt);
}

QVariant GraphObject::getCustomValue(const QString &key) const
{
    auto targetValIt = m_customDataValues.find(key);
    if (targetValIt == m_customDataValues.end()) {
        return {};
    }
    return targetValIt->second;
}

std::map<QString, QVariant> GraphObject::getCustomValueMap() const
{
    return m_customDataValues;
}

}
