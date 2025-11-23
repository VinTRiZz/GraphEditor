CONFIGURE_LIBRARY(GraphWidgets)

COMPONENTS_LINK_COMPONENT(GraphWidgets Logger)
COMPONENTS_LINK_COMPONENT(GraphWidgets Qt-Custom)

LINK_TARGET_LIBRARY(GraphWidgets GraphObject)

LINK_TARGET_LIBRARY(GraphWidgets GraphItems)
LINK_TARGET_LIBRARY(GraphWidgets GraphScene)

LINK_TARGET_LIBRARY(GraphWidgets SavesModule)
LINK_TARGET_LIBRARY(GraphWidgets PluginModule)

# Plugins
target_include_directories(GraphWidgets PRIVATE ../PluginCoreInterface/include)
