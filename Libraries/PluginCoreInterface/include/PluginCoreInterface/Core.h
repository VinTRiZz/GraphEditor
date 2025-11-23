#pragma once

/**
 * @brief Для описания плагина достаточно определить наследника класса AbstractPluginCore
 * @note  Наследник должен называться в соответствии с названием плагина во избежание ошибок
 */

// Макрос для экспорта функций
#ifndef PLUGIN_API_EXPORT
#ifdef _WIN32
    #ifdef DEFAULT_PLUGIN_EXPORT
        #define PLUGIN_API_EXPORT __declspec(dllexport)
    #else
        #define PLUGIN_API_EXPORT __declspec(dllimport)
    #endif
#else
    #define PLUGIN_API_EXPORT __attribute__((visibility("default")))
#endif
#endif // NDEF PLUGIN_API_EXPORT


#include <QString>
#include <map>
#include <list>

namespace Graph {
// Для редактирования свойств виджетов
class PluginItemPropertyWidget;

// Для работы с режимом, определённым в плагине
class PluginInteractionWidget;

// Для работы с конфигурацией плагина
class PluginConfigurationWidget;

class PluginObjectItnterface;

/**
 * @brief The AbstractPluginCore class Базовый класс для работы плагина
 */
class AbstractPluginCore {
public:
    enum class PluginObjectType : int {
        Unknown = -1,
        Vertex,
        Connection,
    };

    bool init();
    virtual bool restoreState() { return true; }
    bool deinit();

    QString getErrorText() { return m_errorText; }

    inline std::list<QString> getObjectNameList() const {
        std::list<QString> res;
        for (auto& [objName, objType] : m_objectTypesMap) {
            res.push_back(objName);
        }
        return res;
    }
    inline PluginObjectType getObjectType(const QString& itemName) { return m_objectTypesMap.at(itemName); }

    virtual Graph::PluginConfigurationWidget* getConfigurationEditor() = 0;
    virtual Graph::PluginItemPropertyWidget* getPropertyEditor(Graph::PluginObjectItnterface* pTarget) = 0;
    virtual Graph::PluginInteractionWidget* getInteractor() = 0;

    virtual Graph::PluginObjectItnterface* createObject(const QString& name) = 0;

private:
    QString m_errorText;
    std::map<QString, PluginObjectType> m_objectTypesMap; // name - type

protected:
    inline void setErrorText(const QString& errText) { m_errorText = errText; }

    inline void registerObject(PluginObjectType objectType, const QString& objectName) {
        m_objectTypesMap.emplace(objectName, objectType);
    }
};

}

// ============================================================ //
// Получение корневого класса плагина

/**
 * @brief Получение инстанции корневого класса плагина
 * @note  Инстанция класса записывается в std::shared_ptr, поэтому лучше не удалять изнутри плагина
 */
extern "C" PLUGIN_API_EXPORT Graph::AbstractPluginCore* createPluginCore();
