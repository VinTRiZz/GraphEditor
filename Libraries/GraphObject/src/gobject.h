#pragma once

#include <QJsonObject>
#include <QString>
#include <QPointF>

#include <unordered_set>

#include "graphcommon.h"

namespace Graph {

class PluginObjectInterface;

/**
 * @brief The GObject class Сущность в графе зависимостей
 */
class GObject {
public:
    bool operator==(const GObject& gObj_) const;
    bool operator!=(const GObject& gObj_) const;

    void setInterface(PluginObjectInterface* pInterface);
    PluginObjectInterface* getInterface() const;

    bool isValid() const;

    graphId_t getId() const;
    void setId(graphId_t newId);

    QString getName() const;
    void setName(const QString& name);

    QPointF getPos() const;
    void setPos(const QPointF& p);

    void addConnection(graphId_t targetId);
    std::unordered_set<graphId_t> getConnections() const;
    void removeConnection(graphId_t targetId);

    QJsonObject getCommonData() const;
    void setCommonData(const QJsonObject &nCommonData);

    QJsonObject getPluginData() const;
    void setPluginData(const QJsonObject &nPluginData);

    QJsonObject getExtraData() const;
    void setExtraData(const QJsonObject &nExtraData);

private:
    graphId_t   m_id {};
    QPointF     m_pos {};
    QString     m_displayName;

    PluginObjectInterface*          m_interface {nullptr};
    std::unordered_set<graphId_t>   m_connections;

    QJsonObject m_commonData;   // Общие данные по объекту (графические, тултип и прочие)
    QJsonObject m_pluginData;   // Данные по самому объекту (относящиеся к PluginObjectInterface)
    QJsonObject m_extraData;    // Дополнительные данные
};

}  // namespace Graph


// Перегрузка для std::unordered_set
template<>
struct std::hash<Graph::GObject>
{
    std::size_t operator()(const Graph::GObject& s) const noexcept {
        return s.getId().value();
    }
};
