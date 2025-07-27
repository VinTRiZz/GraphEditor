# Constants

set(
    RESOURCES_COMMONFILE
    "${CMAKE_CURRENT_SOURCE_DIR}/DATA/common.qrc"
)

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


function(CONFIGURE_LIBRARY LIBRARY_NAME)
    # Files
    file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
    file(GLOB_RECURSE SOURCES "src/*.cpp")
    file(GLOB_RECURSE UIFILES "src/*.ui")
    source_group("Ui Files" FILES ${UIFILES})

    # Library target
    add_library(${LIBRARY_NAME} SHARED)
    target_sources(${LIBRARY_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES} ${RESOURCES_COMMONFILE})

    set(
        TARGET_LINKED_LIBS
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Sql

        -lcrypto
    )

    # Target linking
    target_link_libraries(${LIBRARY_NAME} PRIVATE ${TARGET_LINKED_LIBS} )

    CONFIGURE_SUFFIX(${LIBRARY_NAME} ".so.DEBUG" ".so")

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Error configuring library! Not found sources dir: " "${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    target_include_directories(${LIBRARY_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    message(STATUS "Configured library " ${LIBRARY_NAME})
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

    file(GLOB TESTING_SOURCES "tests/*.cpp" ${RESOURCES_COMMONFILE})
    target_sources(${EDITOR_MODULENAME_TEST} PRIVATE ${TESTING_SOURCES})

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

    # Add test to added list
    set(
        ADDED_EDITOR_TESTS
        ${ADDED_EDITOR_TESTS}
        ${EDITOR_MODULENAME_TEST}
    )
    message(STATUS "Added test: " ${EDITOR_MODULENAME_TEST})
endfunction()
