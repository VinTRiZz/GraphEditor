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

void GObject::addConnection(graphId_t targetId)
{
    assert(targetId.has_value());
    m_connections.insert(targetId.value());
}

std::unordered_set<graphId_t> GObject::getConnections() const
{
    return m_connections;
}

void GObject::removeConnection(graphId_t targetId)
{
    assert(targetId.has_value());
    m_connections.erase(targetId.value());
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
