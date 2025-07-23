CONFIGURE_LIBRARY(CustomWidgets)

## Precompiled headers (build optimisation)
file(GLOB_RECURSE PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}"/src/*.h")
target_precompile_headers(CustomWidgets PRIVATE ${PRECOMPILED_HEADERS})
