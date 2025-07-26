CONFIGURE_LIBRARY(Common)

ADD_EDITOR_TEST(encrypt_test)
LINK_TARGET_LIBRARY(encrypt_test Common)
