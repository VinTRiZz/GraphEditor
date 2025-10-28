#include "vertexplugin.hpp"

#include <filesystem>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>

namespace Graph {

struct VertexPlugin::Impl {
    boost::dll::shared_library pluginLib;

    std::string pluginName;

    Impl() = default;
    ~Impl() {
        if (pluginLib.is_loaded()) {
            pluginLib.unload();
        }
    }

    template<typename Func>
    Func getFunction(const std::string& functionName) {
        if (!pluginLib.is_loaded()) {
            throw std::runtime_error("Plugin not loaded");
        }

        if (!pluginLib.has(functionName)) {
            throw std::runtime_error("Function not found: " + functionName);
        }

        return pluginLib.get<Func>(functionName);
    }
};

VertexPlugin::VertexPlugin(const std::string pluginPath) :
    d {new Impl}
{
    try {
        if (!std::filesystem::exists(pluginPath)) {
            throw std::runtime_error("vertex plugin file not exist: " + pluginPath);
        }

        auto pluginName = std::filesystem::path(pluginPath).filename().string();
        pluginName.erase(pluginName.find(getPluginExtension()) - 1);
        d->pluginName = pluginName;

        d->pluginLib = boost::dll::shared_library(
                    pluginPath,
                    boost::dll::load_mode::append_decorations
                    );
        LOG_OK("Loaded vertex plugin:", d->pluginName);

    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load vertex plugin:", e.what());
        return;
    }
}

bool VertexPlugin::isLoaded() const
{
    return d->pluginLib.is_loaded();
}

VertexItem *VertexPlugin::getVertex(const std::string &plugin, const std::string &vertexType)
{
    auto createFunc = d->getFunction<VertexItem*(*)(const std::string&)>("createVertex");
    return createFunc(vertexType);
}

}
