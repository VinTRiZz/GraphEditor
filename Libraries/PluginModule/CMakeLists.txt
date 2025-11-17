set(PLUGIN_MODULE_LIBS
    boost_system
    boost_filesystem
)

CONFIGURE_LIBRARY(PluginModule ${PLUGIN_MODULE_LIBS})

COMPONENTS_LINK_COMPONENT(PluginModule Logger)
COMPONENTS_LINK_COMPONENT(PluginModule Common)
COMPONENTS_LINK_COMPONENT(PluginModule Qt-Custom)

LINK_TARGET_LIBRARY(PluginModule GraphItems)

ADD_EDITOR_TEST(PluginModule)
