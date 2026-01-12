#include "graphobject.h"

#include <Components/Logger/Logger.h>

#include <QDateTime>
#include <QJsonObject>

#include <QMetaObject>

#include <Components/CustomQt/ObjectView/ObjectView.h>
#include <Components/CustomQt/ObjectView/InternalScene.h>

namespace Graph {

GraphObject::GraphObject(QObject *parent) :
    QObject(parent),
    m_metaInfo(new GraphMetaInformation(this))
{

}

void GraphObject::clearGraphData()
{
    clearVertices();
    clearObjects();
    m_metaInfo->clearData();
}

void GraphObject::synchronizeParents(OVLayers::OVCanvasLayer *pHostView)
{
    LOG_INFO("Parent sync started..."); // TODO: Проработать рекурсивные зависимости ID?

    auto pHostScene = pHostView->getScene();

    LOG_INFO("Objects sync...");
    for (auto* pObj : m_objects) {

        if (pObj->scene() != pHostScene) {
            pHostView->addObject(pObj);
        }
        if (pObj->getParentObjectId() != 0) {
            auto targetParentIt =
                    std::find_if(m_objects.begin(), m_objects.end(),
                                 [childId = pObj->getParentObjectId()](auto* pParentObj){
                return (childId == pParentObj->getPluginObjectId());
            });
            if (targetParentIt != m_objects.end()) {
                pObj->setParentItem(*targetParentIt);
                continue;
            }

            auto targetPluginParentIt =
                    std::find_if(m_pluginObjects.begin(), m_pluginObjects.end(),
                                 [childId = pObj->getParentObjectId()](auto* pParentObj){
                return (childId == pParentObj->getPluginObjectId());
            });
            if (targetPluginParentIt == m_pluginObjects.end()) {
                LOG_WARNING("Did not found parent object with id:", pObj->getParentObjectId());
                continue;
            }

            auto pParent = dynamic_cast<ObjectItems::BasicItem*>(*targetPluginParentIt);
            if (pParent == nullptr) {
                LOG_WARNING("Object is not a scene item with id:", (*targetPluginParentIt)->getPluginObjectId());
            }
            pObj->setParentItem(pParent);
        } else {
            pObj->setParentItem(pHostView->getCanvas());
        }
    }

    LOG_INFO("Extra items sync...");
    for (auto* pObjI : m_pluginObjects) {

        auto pObj = dynamic_cast<ObjectItems::BasicItem*>(pObjI);
        if (nullptr == pObj) {
            LOG_WARNING("Can not synchronize parent for plugin object:", pObjI->getPluginObjectId(), "REASON: Not a item");
            continue;
        }

        if (pObj->scene() != pHostScene) {
            pHostView->addObject(pObj);
        }
        if (pObjI->getParentObjectId() != 0) {
            auto targetParentIt =
                    std::find_if(m_objects.begin(), m_objects.end(),
                                 [childId = pObjI->getParentObjectId()](auto* pParentObj){
                return (childId == pParentObj->getPluginObjectId());
            });
            if (targetParentIt != m_objects.end()) {
                pObj->setParentItem(*targetParentIt);
                continue;
            }

            auto targetPluginParentIt =
                    std::find_if(m_pluginObjects.begin(), m_pluginObjects.end(),
                                 [childId = pObjI->getParentObjectId()](auto* pParentObj){
                return (childId == pParentObj->getPluginObjectId());
            });
            if (targetPluginParentIt == m_pluginObjects.end()) {
                LOG_WARNING("Did not found parent object with id:", pObjI->getParentObjectId());
                continue;
            }
            auto pParent = dynamic_cast<ObjectItems::BasicItem*>(*targetPluginParentIt);
            if (pParent == nullptr) {
                LOG_WARNING("Object is not a scene item with id:", (*targetPluginParentIt)->getPluginObjectId());
            }
            pObj->setParentItem(pParent);
        } else {
            pObj->setParentItem(pHostView->getCanvas());
        }
    }
    LOG_INFO("Parent sync complete");
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
    connect(iVert, &QObject::destroyed,
            this, [this, iVert](){
        removeObject(iVert);
    });
}

const std::unordered_set<GObjectItem*> &GraphObject::getAllObjects() const {
    return m_objects;
}

std::size_t GraphObject::getObjectCount() const {
    return m_objects.size();
}

void GraphObject::removeObject(GObjectItem *iVert)
{
    m_objects.erase(iVert);
    disconnect(iVert, nullptr,  this, nullptr);
}

void GraphObject::clearVertices()
{
    auto objectsCopy = m_objects;
    for (auto* pItem : objectsCopy) {
        removeObject(pItem);
    }
}

void GraphObject::addPluginObject(PluginObjectInterface* pObject)
{
    m_pluginObjects.insert(pObject);

    if (auto pCastedObj = dynamic_cast<QObject*>(pObject); nullptr != pCastedObj) {
        connect(pCastedObj, &QObject::destroyed,
                this, [this, pObject](){
            removePluginObject(pObject);
        });
    }
}

void GraphObject::removePluginObject(PluginObjectInterface *pObject)
{
    m_pluginObjects.erase(pObject);
    if (auto pCastedObj = dynamic_cast<QObject*>(pObject); nullptr != pCastedObj) {
        disconnect(pCastedObj, nullptr, this, nullptr);
    }
}

const std::unordered_set<PluginObjectInterface *> &GraphObject::getPluginObjects() const
{
    return m_pluginObjects;
}

void GraphObject::clearObjects()
{
    auto objectsCopy = m_pluginObjects;
    for (auto* pObject : objectsCopy) {
        removePluginObject(pObject);
    }
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

void GraphMetaInformation::clearData()
{
    m_name.clear();
    m_description.clear();
    m_createTime = {};
    m_editTime = {};
    m_savepath.clear();
}

void GraphObjectUser::setGraph(const Graph::GraphObjectManagerPtr& pGraph) {
    auto pPrevGraph = m_pGraph;
    m_pGraph = pGraph;
    processGraphChange(pPrevGraph);
}

bool GraphObjectUser::isGraphSet() const
{
    return (m_pGraph && (m_pGraph->getObject() != nullptr));
}

Graph::GraphObjectManagerPtr GraphObjectUser::getGraph() const {
    return m_pGraph;
}

}  // namespace Graph
