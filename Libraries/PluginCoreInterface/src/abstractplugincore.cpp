#include "abstractplugincore.hpp"

namespace Graph {

bool AbstractPluginCore::restoreState() { return true; }

QString AbstractPluginCore::getErrorText() { return m_errorText; }

std::list<QString> AbstractPluginCore::getObjectNameList() const {
    std::list<QString> res;
    for (auto& [objName, objType] : m_objectTypesMap) {
        res.push_back(objName);
    }
    return res;
}

AbstractPluginCore::PluginObjectType AbstractPluginCore::getObjectType(const QString &itemName) { return m_objectTypesMap.at(itemName); }

void AbstractPluginCore::setErrorText(const QString &errText) { m_errorText = errText; }

void AbstractPluginCore::registerObject(PluginObjectType objectType, const QString &objectName) {
    m_objectTypesMap.emplace(objectName, objectType);
}

}
