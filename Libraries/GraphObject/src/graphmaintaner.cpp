#include "graphmaintaner.h"

namespace Graph {

GraphMaintainer::GraphMaintainer(QObject* parent) : QObject(parent) {}
GraphMaintainer::~GraphMaintainer() {}

bool GraphMaintainer::operator==(const GraphMaintainer& gObj_) const {
    if (m_graph != gObj_.m_graph) {
        LOG_INFO("GraphMaintainer::equal objects are not equal");
        return false;
    }

    for (const auto& customProp : m_customDataValues) {
        auto gObjValue = gObj_.m_customDataValues.find(customProp.first);
        if (gObjValue == gObj_.m_customDataValues.end()) {
            LOG_INFO("GraphMaintainer::equal custom value not exist");
            return false;
        }

        if (customProp.second != gObjValue->second) {
            LOG_INFO("GraphMaintainer::equal custom values not equal:",
                     customProp.first);
            return false;
        }
    }

    if ((m_name != gObj_.m_name) || (m_description != gObj_.m_description) ||
        (m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT) !=
         gObj_.m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT)) ||
        (m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT) !=
         gObj_.m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT))) {
        // Закомментил на будущее
        //        LOG_DEBUG("GraphMaintainer::equal common", m_name !=
        //        gObj_.m_name, m_description != gObj_.m_description,
        //                               m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT)
        //                               !=
        //                               gObj_.m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT),
        //                               m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT)
        //                               !=
        //                               gObj_.m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT));

        //        LOG_DEBUG("GraphMaintainer::equal common data my:",
        //                  m_name,
        //                  m_description,
        //                  m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT),
        //                  m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT));

        //        LOG_DEBUG("GraphMaintainer::equal common data other:",
        //                  gObj_.m_name,
        //                  gObj_.m_description,
        //                  gObj_.m_createTime.toString(GraphCommon::DATE_CONVERSION_FORMAT),
        //                  gObj_.m_editTime.toString(GraphCommon::DATE_CONVERSION_FORMAT));
        return false;
    }
    return true;
}

bool GraphMaintainer::operator==(const PMaintainer& gObj_) const {
    return (*this == *gObj_);
}

bool GraphMaintainer::operator!=(const GraphMaintainer& gObj_) const {
    return !(*this == gObj_);
}

bool GraphMaintainer::operator!=(const PMaintainer& gObj_) const {
    return (*this != *gObj_);
}

PMaintainer GraphMaintainer::createInstance() {
    return std::make_shared<GraphMaintainer>();
}

GraphObject& GraphMaintainer::getObject() {
    return m_graph;
}

GraphExtendedObject* GraphMaintainer::getExtendedObject() {
    return &m_graph;
}

void GraphMaintainer::resetMaintainer() {
    m_graph.clearConnections();
    m_graph.clearVertices();
    m_createTime = {};
    m_editTime = {};
    m_customDataValues.clear();
    m_name.clear();
    m_description.clear();
}

void GraphMaintainer::setName(const QString& iName) {
    m_name = iName;
    emit changedCommonProperty();
}

QString GraphMaintainer::getName() const {
    return m_name;
}

void GraphMaintainer::setDescription(const QString& iDescr) {
    m_description = iDescr;
    emit changedCommonProperty();
}

QString GraphMaintainer::getDescription() const {
    return m_description;
}

void GraphMaintainer::setCreateTime(const QDateTime& iDateTime) {
    m_createTime = iDateTime;
    emit changedCommonProperty();
}

QDateTime GraphMaintainer::getCreateTime() const {
    return m_createTime;
}

void GraphMaintainer::setEditTime(const QDateTime& iDateTime) {
    m_editTime = iDateTime;
    emit changedCommonProperty();
}

QDateTime GraphMaintainer::getEditTime() const {
    return m_editTime;
}

void GraphMaintainer::setCustomValue(const QString& key,
                                     const QVariant& value) {
    m_customDataValues[key] = value;
    emit changedCustomProperty();
}

void GraphMaintainer::removeCustomValue(const QString& key) {
    auto targetValIt = m_customDataValues.find(key);
    if (targetValIt == m_customDataValues.end()) {
        return;
    }
    m_customDataValues.erase(targetValIt);
    emit changedCustomProperty();
}

QVariant GraphMaintainer::getCustomValue(const QString& key) const {
    auto targetValIt = m_customDataValues.find(key);
    if (targetValIt == m_customDataValues.end()) {
        return {};
    }
    return targetValIt->second;
}

std::map<QString, QVariant> GraphMaintainer::getCustomValueMap() const {
    return m_customDataValues;
}

const std::map<QString, QVariant> &GraphMaintainer::getCustomValueMapRef() const
{
    return m_customDataValues;
}

}  // namespace Graph
