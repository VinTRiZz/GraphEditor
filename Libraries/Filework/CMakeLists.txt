CONFIGURE_LIBRARY(Filework)

LINK_TARGET_LIBRARY(Filework Logger)
LINK_TARGET_LIBRARY(Filework Qt-Custom)
LINK_TARGET_LIBRARY(Filework AppInfrastructure)
LINK_TARGET_LIBRARY(Filework GraphObject)

ADD_EDITOR_TEST(Filework)
