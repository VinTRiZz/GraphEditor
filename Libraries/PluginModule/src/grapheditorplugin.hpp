#pragma once

#include <string>
#include <memory>
#include <list>

#include <QString>

namespace Graph {

class AbstractPluginCore;
class PluginLibraryManager;

class GraphEditorPlugin
{
    std::shared_ptr<AbstractPluginCore> m_pPluginCore;
    std::shared_ptr<PluginLibraryManager> m_libraryManager;
public:
    GraphEditorPlugin();
    virtual ~GraphEditorPlugin();

    bool initFromFile(const QString& pluginFile);
    QString getPluginName() const;

    std::shared_ptr<AbstractPluginCore> getPluginCore() const;

protected:
    std::shared_ptr<PluginLibraryManager> getLibraryManager() const;
};

} // namespace Graph

