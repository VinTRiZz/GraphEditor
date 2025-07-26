CONFIGURE_LIBRARY(GraphObject)

LINK_TARGET_LIBRARY(GraphObject Common)

# Precompiled headers (build optimisation)
file(GLOB_RECURSE PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}"/src/*.h")
target_precompile_headers(CustomWidgets PRIVATE ${PRECOMPILED_HEADERS})

ADD_EDITOR_TEST(graph_object_test)
LINK_TARGET_LIBRARY(graph_object_test GraphObject)
