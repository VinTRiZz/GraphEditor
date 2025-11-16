CONFIGURE_LIBRARY(PluginModule)

LINK_TARGET_LIBRARY(PluginModule Logger)
LINK_TARGET_LIBRARY(PluginModule Common)
LINK_TARGET_LIBRARY(PluginModule Qt-Custom)

LINK_TARGET_LIBRARY(PluginModule GraphItems)

ADD_EDITOR_TEST(PluginModule)
