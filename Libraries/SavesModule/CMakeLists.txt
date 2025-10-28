CONFIGURE_LIBRARY(SavesModule)

LINK_TARGET_LIBRARY(SavesModule Logger)
LINK_TARGET_LIBRARY(SavesModule Encryption)
LINK_TARGET_LIBRARY(SavesModule Qt-Custom)
LINK_TARGET_LIBRARY(SavesModule GraphObject)

ADD_EDITOR_TEST(SavesModule)
