#include "pluginobjectitnterface.hpp"

namespace Graph {

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
    return m_metadata.pluginName;
}

QJsonObject PluginObjectInterface::toJson() const
{
    if (!getPluginObjectId().has_value()) {
        return {};
    }
    QJsonObject vObj;
    vObj["id"] = QString::number(getPluginObjectId().value());
    vObj["pluginName"] = getPluginName();
    vObj["pluginObjectName"] = getPluginObjectName();
    return vObj;
}

bool PluginObjectInterface::fromJson(const QJsonObject &arr)
{
    if (arr.contains("id")) {
        setPluginObjectId(arr["id"].toInt());
    }

    if (arr.contains("pluginName")) {
        setPluginName(arr["pluginName"].toString());
    }

    if (arr.contains("pluginObjectName")) {
        setPluginObjectName(arr["pluginObjectName"].toString());
    }

    return getPluginObjectId().has_value();
}

const PluginObjectInterface::ObjectMetadata &PluginObjectInterface::getMetadata() const
{
    return m_metadata;
}

bool PluginObjectInterface::ObjectMetadata::operator <(const ObjectMetadata &iv) const {
    if (!iv.objectId.has_value() && iv.objectId.has_value()) {
        return true;
    }

    if (iv.objectId.has_value() && !iv.objectId.has_value()) {
        return false;
    }

    return iv.objectId.value() < objectId.value();
}

bool PluginObjectInterface::ObjectMetadata::operator ==(const ObjectMetadata &iv) const {
    return  objectId == iv.objectId &&
            pluginName == iv.pluginName &&
            pluginObjectName == iv.pluginObjectName;
}

} // namespace Graph
