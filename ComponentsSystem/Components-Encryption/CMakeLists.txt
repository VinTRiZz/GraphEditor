COMPONENTS_CONFIGURE_COMPONENT(Encryption "1.0.0"
    crypto
)
option(COMPONENTS_SW_encryption_rsa "" OFF)
option(COMPONENTS_SW_encryption_chacha "" OFF)
COMPONENTS_LINK_COMPONENT(Components-Encryption ExtraClasses)
COMPONENTS_LINK_COMPONENT(Components-Encryption Filework)
if (COMPONENTS_SW_encryption_rsa)
    target_compile_definitions(Components-Encryption PRIVATE COMPONENTS_SW_encryption_rsa)
    COMPONENTS_LINK_COMPONENT(Components-Encryption ExtraClasses)
    COMPONENTS_LINK_COMPONENT(Components-Encryption Filework)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Encryption
        "${CMAKE_CURRENT_SOURCE_DIR}/src/rsa.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/rsa.cpp"
    )
endif()
if (COMPONENTS_SW_encryption_chacha)
    target_compile_definitions(Components-Encryption PRIVATE COMPONENTS_SW_encryption_chacha)
    target_link_libraries(Components-Encryption PRIVATE sodium)
else()
    COMPONENTS_REMOVE_SOURCES(Components-Encryption
        "${CMAKE_CURRENT_SOURCE_DIR}/src/chacha20.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/chacha20.cpp"
    )
endif()
COMPONENTS_ADD_COMPONENT_TEST(Encryption)
