#pragma once

#include <string>
#include <memory>
#include <list>

namespace Graph {

// Для редактирования свойств виджетов
class PluginItemPropertyWidget;

// Для работы с режимом, определённым в плагине
class PluginInteractionWidget;

// Для работы с конфигурацией плагина
class PluginConfigurationWidget;

// Базовый класс для объектов сцены, которые может дать плагин
class PluginObjectItnterface;


class PluginLibraryManager;

class GraphEditorPlugin
{
    std::shared_ptr<PluginLibraryManager> m_libraryManager;
public:
    GraphEditorPlugin();
    virtual ~GraphEditorPlugin();

    bool initFromFile(const std::string& pluginFile);
    std::string getPluginName() const;

    std::list<std::string> getObjectNameList() const;

    Graph::PluginObjectItnterface* createObject(const std::string& name);

    Graph::PluginConfigurationWidget* getConfigurationEditor();
    Graph::PluginItemPropertyWidget* getPropertyEditor(Graph::PluginObjectItnterface* pTarget);
    Graph::PluginInteractionWidget* getInteractor();

protected:
    std::shared_ptr<PluginLibraryManager> getLibraryManager() const;
};

} // namespace Graph

