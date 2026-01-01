#pragma once

/**
 * @brief Для описания плагина достаточно определить наследника класса AbstractPluginCore
 * @note  Наследник должен называться в соответствии с названием плагина во избежание ошибок
 */

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

class PluginObjectInterface;

/**
 * @brief The AbstractPluginCore class Базовый класс для работы плагина
 */
class AbstractPluginCore {
    QString m_pluginName;
public:
    explicit AbstractPluginCore(const QString& pluginName) : m_pluginName {pluginName} {}

    enum class PluginObjectType : int {
        Unknown = -1,
        Vertex,
        Connection,
    };

    bool init();
    virtual bool restoreState();
    bool deinit();

    QString getErrorText();

    std::list<QString> getObjectNameList() const;
    PluginObjectType getObjectType(const QString& itemName);

    virtual Graph::PluginConfigurationWidget* getConfigurationEditor() = 0;
    virtual Graph::PluginItemPropertyWidget* getPropertyEditor(Graph::PluginObjectInterface* pTarget) = 0;
    virtual Graph::PluginInteractionWidget* getInteractor() = 0;

    virtual Graph::PluginObjectInterface* createObject(const QString& name) = 0;

private:
    QString m_errorText;
    std::map<QString, PluginObjectType> m_objectTypesMap; // name - type

protected:
    void setErrorText(const QString& errText);

    void registerObject(PluginObjectType objectType, const QString& objectName);
};

}
