#pragma once

#include <QString>
#include <QJsonObject>

#include <hash_fun.h>

#include "graphcommon.h"

namespace Graph {

class PluginObjectInterface
{
public:
    PluginObjectInterface() = default;
    virtual ~PluginObjectInterface() = default;

    void setParentObjectId(const graphId_t& id);
    graphId_t getParentObjectId() const;

    void setPluginObjectId(const graphId_t& id);
    graphId_t getPluginObjectId() const;

    void setPluginName(const QString& pluginName);
    QString getPluginName() const;

    void setPluginObjectName(const QString& pluginObjectName);
    QString getPluginObjectName() const;

    virtual QJsonObject toJson() const;
    virtual bool fromJson(const QJsonObject& arr);

    struct ObjectMetadata {
        graphId_t objectId;
        graphId_t parentObjectId {0};
        QString pluginName;
        QString pluginObjectName;
        QJsonObject pluginObjectData;

        bool operator <(const ObjectMetadata& iv) const;
        bool operator ==(const ObjectMetadata& iv) const;
    };
    const ObjectMetadata& getMetadata() const;

private:
    ObjectMetadata m_metadata;
};

} // namespace Graph

// Определение std::hash для работы с std::unordered_set
namespace std {
template<>
struct hash<Graph::PluginObjectInterface::ObjectMetadata>
{
    std::size_t operator()(const Graph::PluginObjectInterface::ObjectMetadata& metaData) const noexcept
    {
        return hash<long long>{}(metaData.objectId);
    }
};
}
