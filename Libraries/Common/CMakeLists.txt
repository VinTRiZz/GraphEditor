CONFIGURE_LIBRARY(Common)

ADD_EDITOR_TEST(Common)

file(GLOB PRECOMP_HDRS "src/*.h")
target_precompile_headers(Common PRIVATE ${PRECOMP_HDRS})
