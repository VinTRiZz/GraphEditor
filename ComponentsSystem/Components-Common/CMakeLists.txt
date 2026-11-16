COMPONENTS_CONFIGURE_COMPONENT(Common "1.0.0")
target_precompile_headers(Components-Common PUBLIC
    "src/appsettings/appsetting.hpp"
    "src/directorymanager.hpp"
    "src/utility.hpp"
)
COMPONENTS_LINK_COMPONENT(Components-Common ExtraClasses)
COMPONENTS_LINK_COMPONENT(Components-Common Filework)
COMPONENTS_LINK_COMPONENT(Components-Common Logger)
if (COMPONENTS_IS_ENABLED_QT)
    target_link_libraries(Components-Common PRIVATE Qt5::Qt5::Gui Qt5::Widgets)
endif()
COMPONENTS_ADD_COMPONENT_TEST(Common)
