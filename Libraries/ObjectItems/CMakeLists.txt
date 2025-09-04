CONFIGURE_LIBRARY(ObjectItems)

ADD_EDITOR_TEST(ObjectItems)

LINK_TARGET_LIBRARY(ObjectItems Common)

file(GLOB_RECURSE PRECOMP_HDRS "src/*.h")
target_precompile_headers(Common PRIVATE ${PRECOMP_HDRS})
