CONFIGURE_LIBRARY(Filework)

LINK_TARGET_LIBRARY(Filework AppInfrastructure)
LINK_TARGET_LIBRARY(Filework GraphObject)
LINK_TARGET_LIBRARY(Filework Qt-Custom)

ADD_EDITOR_TEST(Filework)
