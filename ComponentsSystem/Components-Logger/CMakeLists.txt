COMPONENTS_CONFIGURE_COMPONENT(Logger "1.0.0")
target_precompile_headers(Components-Logger PUBLIC
    "src/common.hpp"
    "src/filewriterbase.hpp"
    "src/instancebase.hpp"
)
if (COMPONENTS_IS_ENABLED_QT)
    target_link_libraries(Components-Logger PRIVATE Qt5::)
endif()
COMPONENTS_ADD_COMPONENT_TEST(Logger)
