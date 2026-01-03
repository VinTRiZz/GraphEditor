#include "pluginobjectitnterface.hpp"

#include <Components/Logger/Logger.h>

namespace Graph {

void PluginObjectInterface::setParentObjectId(const graphId_t &id)
{
    m_metadata.parentObjectId = id;
}

graphId_t PluginObjectInterface::getParentObjectId() const
{
    return m_metadata.parentObjectId;
}

void PluginObjectInterface::setPluginObjectId(const graphId_t &id)
{
    m_metadata.objectId = id;
}

graphId_t PluginObjectInterface::getPluginObjectId() const
{
    return m_metadata.objectId;
}

void PluginObjectInterface::setPluginName(const QString &pluginName)
{
    m_metadata.pluginName = pluginName;
}

QString PluginObjectInterface::getPluginName() const
{
    return m_metadata.pluginName;
}

void PluginObjectInterface::setPluginObjectName(const QString &pluginObjectName)
{
    m_metadata.pluginObjectName = pluginObjectName;
}

QString PluginObjectInterface::getPluginObjectName() const
{
    return m_metadata.pluginObjectName;
}

QJsonObject PluginObjectInterface::toJson() const
{
    if (getPluginObjectId() == 0) {
        return {};
    }
    QJsonObject vObj;
    vObj["id"] = QString::number(getPluginObjectId());
    vObj["parentId"] = QString::number(getParentObjectId());
    vObj["pluginName"] = getPluginName();
    vObj["pluginObjectName"] = getPluginObjectName();
    return vObj;
}

bool PluginObjectInterface::fromJson(const QJsonObject &arr)
{
    if (arr.contains("id")) {
        setPluginObjectId(arr["id"].toString().toLongLong());
    }

    if (arr.contains("parentId")) {
        setParentObjectId(arr["parentId"].toString().toLongLong());
    }

    return (0 != getPluginObjectId());
}

const PluginObjectInterface::ObjectMetadata &PluginObjectInterface::getMetadata() const
{
    return m_metadata;
}

bool PluginObjectInterface::ObjectMetadata::operator <(const ObjectMetadata &iv) const {
    return iv.objectId < objectId;
}

bool PluginObjectInterface::ObjectMetadata::operator ==(const ObjectMetadata &iv) const {
    return  objectId == iv.objectId &&
            pluginName == iv.pluginName &&
            pluginObjectName == iv.pluginObjectName &&
            pluginObjectData == iv.pluginObjectData;
}

} // namespace Graph
