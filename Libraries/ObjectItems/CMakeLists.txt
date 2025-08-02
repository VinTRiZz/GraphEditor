CONFIGURE_LIBRARY(ObjectItems)

LINK_TARGET_LIBRARY(ObjectItems Common)
LINK_TARGET_LIBRARY(ObjectItems GraphObject)

file(GLOB_RECURSE PRECOMP_HDRS "src/*.h")
target_precompile_headers(Common PRIVATE ${PRECOMP_HDRS})
