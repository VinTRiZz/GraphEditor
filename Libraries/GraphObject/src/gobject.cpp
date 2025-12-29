#include "gobject.h"

#include <Components/Common/CommonFunctions.h>
#include <Components/Logger/Logger.h>

#include "pluginobjectitnterface.hpp"

namespace Graph {

bool GObject::operator==(const GObject& gObj_) const {
    return (std::tie(m_id, m_interface, m_connections, m_pluginData, m_extraData) ==
            std::tie(gObj_.m_id, gObj_.m_interface, gObj_.m_connections, gObj_.m_pluginData, gObj_.m_extraData));
}

bool GObject::operator!=(const GObject& gObj_) const {
    return !(*this == gObj_);
}

void GObject::setInterface(PluginObjectInterface *pInterface)
{
    m_interface = pInterface;
}

PluginObjectInterface *GObject::getInterface() const
{
    return m_interface;
}

bool GObject::isValid() const {
    return m_id.has_value();
}

graphId_t GObject::getId() const
{
    return m_id;
}

void GObject::setId(graphId_t newId)
{
    m_id = newId;
}

QString GObject::getName() const
{
    return m_displayName;
}

void GObject::setName(const QString &name)
{
    m_displayName = name;
}

QPointF GObject::getPos() const
{
    return m_pos;
}

void GObject::setPos(const QPointF &p)
{
    m_pos = p;
}

void GObject::addConnection(const GConnection_t &conInfo)
{
    assert(conInfo.first.has_value());
    m_connections.push_back(conInfo);
}

std::list<std::pair<graphId_t, ObjectItems::objectId_t> > GObject::getConnections() const
{
    return m_connections;
}

void GObject::removeConnection(ObjectItems::objectId_t connectionItemId)
{
    auto conInfoIt = std::find_if(m_connections.begin(), m_connections.end(), [connectionItemId](auto& conInfo){
        return conInfo.second == connectionItemId;
    });
    if (conInfoIt == m_connections.end()) {
        return;
    }
    m_connections.erase(conInfoIt);
}

QJsonObject GObject::getCommonData() const
{
    return m_commonData;
}

void GObject::setCommonData(const QJsonObject &nCommonData)
{
    m_commonData = nCommonData;
}

QJsonObject GObject::getPluginData() const
{
    return m_pluginData;
}

void GObject::setPluginData(const QJsonObject &nPluginData)
{
    m_pluginData = nPluginData;
}

QJsonObject GObject::getExtraData() const
{
    return m_extraData;
}

void GObject::setExtraData(const QJsonObject &nExtraData)
{
    m_extraData = nExtraData;
}

}
