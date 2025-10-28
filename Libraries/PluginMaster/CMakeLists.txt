CONFIGURE_LIBRARY(PluginMaster)

LINK_TARGET_LIBRARY(PluginMaster Logger)
LINK_TARGET_LIBRARY(PluginMaster Common)
LINK_TARGET_LIBRARY(PluginMaster Qt-Custom)

LINK_TARGET_LIBRARY(PluginMaster GraphItems)

ADD_EDITOR_TEST(PluginMaster)
