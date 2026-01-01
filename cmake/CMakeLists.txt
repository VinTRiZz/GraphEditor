
# Suffix configuration function
function(CONFIGURE_SUFFIX TARGET_NAME TARGET_DBG_SUFF TARGET_REL_SUFF)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${TARGET_NAME} PRIVATE DEBUG_MODE)
        target_compile_options(${TARGET_NAME} PRIVATE -g)
        set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ${TARGET_DBG_SUFF})
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${TARGET_NAME} PRIVATE NDEBUG)
        target_compile_options(${TARGET_NAME} PRIVATE -O3)
        set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ${TARGET_REL_SUFF})
    endif()
endfunction()

# Common library configuration function
function(CONFIGURE_LIBRARY LIBRARY_NAME)
    # Files
    file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
    file(GLOB_RECURSE SOURCES "src/*.cpp")
    file(GLOB_RECURSE UIFILES "src/*.ui")
    source_group("Ui Files" FILES ${UIFILES})

    # Library target
    add_library(${LIBRARY_NAME} SHARED)
    target_sources(${LIBRARY_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES})

    install(TARGETS ${LIBRARY_NAME}
        RUNTIME     DESTINATION ${INSTALL_DESTINATION_PREFIX}
        LIBRARY     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib
        ARCHIVE     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib/static
        INCLUDES    DESTINATION ${INSTALL_DESTINATION_PREFIX}/include
    )

    set(
        TARGET_LINKED_LIBS
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        ${ARGN}
    )

    # Target linking
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${TARGET_LINKED_LIBS} )

    CONFIGURE_SUFFIX(${LIBRARY_NAME} ".so.DEBUG" ".so")

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Error configuring library! Not found sources dir: " "${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    target_include_directories(${LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories(${LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    message(STATUS "Configured library " ${LIBRARY_NAME})
endfunction()


# Plugin configuration function
function(CONFIGURE_PLUGIN PLUGIN_NAME)

    # Files
    file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
    file(GLOB_RECURSE SOURCES "src/*.cpp")
    file(GLOB_RECURSE UIFILES "src/*.ui")
    source_group("Ui Files" FILES ${UIFILES})

    # Plugin target
    add_library(${PLUGIN_NAME} SHARED)
    target_sources(${PLUGIN_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES})
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        OUTPUT_NAME "${PLUGIN_NAME}"
        PREFIX ""
        SUFFIX ".gepl"
    )

    # Export files (windows things)
    if(WIN32)
        set_target_properties(${PLUGIN_NAME} PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()

    install(TARGETS ${PLUGIN_NAME}
        LIBRARY     DESTINATION ${INSTALL_DESTINATION_PREFIX}/plugins
    )

    target_compile_definitions(${PLUGIN_NAME} PRIVATE
        PLUGIN_NAME="${PLUGIN_NAME}"
    )

    set(
        TARGET_LINKED_LIBS
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        ${ARGN}
    )

    # Target linking
    target_link_libraries(${PLUGIN_NAME} PRIVATE ${TARGET_LINKED_LIBS} )
    LINK_TARGET_LIBRARY(${PLUGIN_NAME} PluginCoreInterface)

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Error configuring plugin! Not found sources dir: " "${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    target_include_directories(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    message(STATUS "Configured plugin " ${PLUGIN_NAME})
endfunction()


# For linking
set(
    LIBRARIES_BASEDIR
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries
)
function(LINK_TARGET_LIBRARY TARGET_NAME LIBRARY_NAME)
    set(
        LTL_LIBRARY_INCLUDE_DIR
        "${LIBRARIES_BASEDIR}/${LIBRARY_NAME}/include"
    )

    if(NOT EXISTS ${LTL_LIBRARY_INCLUDE_DIR})
        set(
            LTL_LIBRARY_INCLUDE_DIR
            "${LIBRARIES_BASEDIR}/Components-${LIBRARY_NAME}/include"
        )
    endif()

    if(NOT EXISTS ${LTL_LIBRARY_INCLUDE_DIR})
        message(FATAL_ERROR "Error searching for ${LIBRARY_NAME} (not exist)")
    endif()

    target_link_libraries(
        ${TARGET_NAME} PRIVATE
        ${LIBRARY_NAME}
    )

    target_include_directories(
        ${TARGET_NAME} PUBLIC
        ${LTL_LIBRARY_INCLUDE_DIR}
    )
    message(STATUS "Linking target " ${TARGET_NAME} " : added lib " ${LIBRARY_NAME})
endfunction()



function(ADD_EDITOR_TEST EDITOR_MODULENAME)
    set(EDITOR_MODULENAME_TEST ${EDITOR_MODULENAME}_TEST)

    add_executable(${EDITOR_MODULENAME_TEST})

    file(GLOB TESTING_SOURCES "tests/*.cpp")
    target_sources(${EDITOR_MODULENAME_TEST} PRIVATE ${TESTING_SOURCES})
    target_include_directories(${EDITOR_MODULENAME_TEST} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    get_target_property(EDITOR_MODULE_LIBS ${EDITOR_MODULENAME} LINK_LIBRARIES)
    target_link_libraries(
        ${EDITOR_MODULENAME_TEST} PRIVATE
        GTest::gtest_main
        GTest::gtest
        ${EDITOR_MODULE_LIBS}
    )

    get_target_property(EDITOR_MODULE_INCLUDES ${EDITOR_MODULENAME} INCLUDE_DIRECTORIES)
    target_include_directories(
        ${EDITOR_MODULENAME_TEST} PUBLIC
        ${EDITOR_MODULE_INCLUDES}
    )

    include(GoogleTest)
    gtest_discover_tests(${EDITOR_MODULENAME_TEST})

    # Надо же ему что-то использовать из модуля
    LINK_TARGET_LIBRARY(${EDITOR_MODULENAME_TEST} ${EDITOR_MODULENAME})
    message(STATUS "Added test: " ${EDITOR_MODULENAME_TEST})
endfunction()
