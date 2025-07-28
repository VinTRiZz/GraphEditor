CONFIGURE_LIBRARY(ObjectScene)

LINK_TARGET_LIBRARY(ObjectScene Common)
LINK_TARGET_LIBRARY(ObjectScene GraphObject)

file(GLOB_RECURSE PRECOMP_HDRS "src/*.h")
target_precompile_headers(Common PRIVATE ${PRECOMP_HDRS})
