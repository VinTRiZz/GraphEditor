CONFIGURE_LIBRARY(GraphObject)

LINK_TARGET_LIBRARY(GraphObject Logger)
LINK_TARGET_LIBRARY(GraphObject Qt-Custom)
LINK_TARGET_LIBRARY(GraphObject AppInfrastructure)

ADD_EDITOR_TEST(GraphObject)
