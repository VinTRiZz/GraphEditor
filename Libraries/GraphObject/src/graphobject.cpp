#include "graphobject.h"

#include <Components/Logger/Logger.h>

#include <QDateTime>
#include <QJsonObject>

#include <QMetaObject>

namespace Graph {

GraphObject::GraphObject(QObject *parent) :
    QObject(parent),
    m_metaInfo(new GraphMetaInformation(this))
{

}

void GraphObject::clearGraphData()
{
    clearVertices();
    m_metaInfo = {};
}

bool GraphObject::operator==(const GraphObject& gObj_) const {
    if (m_objects != gObj_.m_objects) {
        return false;
    }

    if (m_metaInfo != gObj_.m_metaInfo) {
        return false;
    }
    return true;
}

bool GraphObject::operator!=(const GraphObject& gObj_) const {
    return !(*this == gObj_);
}

GraphMetaInformation *GraphObject::getMetaInfo() const
{
    return m_metaInfo;
}

void GraphObject::addObject(GObjectItem *iVert) {
    m_objects.insert(iVert);
}

const std::unordered_set<GObjectItem*> &GraphObject::getAllObjects() const {
    return m_objects;
}

std::size_t GraphObject::getObjectCount() const {
    return m_objects.size();
}

void GraphObject::removeObject(graphId_t vertexId)
{
    
}

void GraphObject::clearVertices()
{
    m_objects.clear();
}

void GraphObject::addPluginObject(PluginObjectInterface* pObject)
{
    m_pluginObjects.insert(pObject);
}

void GraphObject::removePluginObject(PluginObjectInterface *pObject)
{
    m_pluginObjects.erase(pObject);
}

const std::unordered_set<PluginObjectInterface *> &GraphObject::getPluginObjects() const
{
    return m_pluginObjects;
}

void GraphObject::clearObjects()
{
    m_pluginObjects.clear();
}



void GraphObjectManager::setObject(GraphObject *pObject)
{
    if (m_graphObject) {
        QObject::disconnect(m_destroyedConnection);
    }
    m_graphObject = pObject;

    if (nullptr != m_graphObject) {
        m_destroyedConnection = QObject::connect(m_graphObject, &QObject::destroyed,
                                                 [this](){
            m_destroyedConnection = {};
            m_graphObject = nullptr;
        });
    }
}

GraphObject *GraphObjectManager::getObject()
{
    return m_graphObject;
}

GraphObjectManagerPtr GraphObjectManager::createGraphInstance()
{
    auto pRes = std::make_shared<GraphObjectManager>();
    pRes->setObject(new GraphObject);
    return pRes;
}

QString GraphMetaInformation::getName() const
{
    return m_name;
}

void GraphMetaInformation::setName(const QString &newName)
{
    m_name = newName;
    emit dataChanged();
}

QString GraphMetaInformation::getDescription() const
{
    return m_description;
}

void GraphMetaInformation::setDescription(const QString &newDescription)
{
    m_description = newDescription;
    emit dataChanged();
}

QDateTime GraphMetaInformation::getCreateTime() const
{
    return m_createTime;
}

void GraphMetaInformation::setCreateTime(const QDateTime &newCreateTime)
{
    m_createTime = newCreateTime;
    emit dataChanged();
}

QDateTime GraphMetaInformation::getEditTime() const
{
    return m_editTime;
}

void GraphMetaInformation::setEditTime(const QDateTime &newEditTime)
{
    m_editTime = newEditTime;
    emit dataChanged();
}

QString GraphMetaInformation::getSavepath() const
{
    return m_savepath;
}

void GraphMetaInformation::setSavepath(const QString &newSavepath)
{
    m_savepath = newSavepath;
    emit dataChanged();
}

bool GraphMetaInformation::operator==(const GraphMetaInformation &gObj_) const
{
    return (std::tie(m_name, m_description, m_createTime, m_editTime, m_savepath) ==
            std::tie(gObj_.m_name, gObj_.m_description, gObj_.m_createTime, gObj_.m_editTime, gObj_.m_savepath));
}

bool GraphMetaInformation::operator!=(const GraphMetaInformation &gObj_) const
{
    return !(*this == gObj_);
}

void GraphObjectUser::setGraph(const Graph::GraphObjectManagerPtr& pGraph) {
    m_pGraph = pGraph;
    processGraphChange();
}

bool GraphObjectUser::isGraphSet() const
{
    return (m_pGraph && (m_pGraph->getObject() != nullptr));
}

Graph::GraphObjectManagerPtr GraphObjectUser::getGraph() const {
    return m_pGraph;
}

}  // namespace Graph
