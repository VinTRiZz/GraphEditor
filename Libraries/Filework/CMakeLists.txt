CONFIGURE_LIBRARY(Filework)

LINK_TARGET_LIBRARY(Filework Common)
LINK_TARGET_LIBRARY(Filework GraphObject)

ADD_EDITOR_TEST(save_load_test)
LINK_TARGET_LIBRARY(save_load_test GraphObject)
LINK_TARGET_LIBRARY(save_load_test Filework)

ADD_EDITOR_TEST(gse_test)
LINK_TARGET_LIBRARY(gse_test GraphObject)
LINK_TARGET_LIBRARY(gse_test Filework)
