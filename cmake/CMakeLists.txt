# Constants

set(
    RESOURCES_COMMONFILE
    "${CMAKE_CURRENT_SOURCE_DIR}/DATA/common.qrc"
)




function(CONFIGURE_LIBRARY_NOQT LIBRARY_NAME)
    # Files
    file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
    file(GLOB_RECURSE SOURCES "src/*.cpp")

    # Library target
    add_library(${LIBRARY_NAME} SHARED)
    target_sources(${LIBRARY_NAME} PRIVATE ${HEADERS} ${SOURCES})

    # Runtime linking of target
    set_target_properties(
        ${LIBRARY_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH TRUE
        INSTALL_RPATH "./lib"
    )

    # Build flag configuration
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${LIBRARY_NAME} PRIVATE DEBUG_MODE)
        target_compile_options(${LIBRARY_NAME} PRIVATE -g)
        set_target_properties(
            ${LIBRARY_NAME}
            PROPERTIES
            SUFFIX ".DEBUG"
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${LIBRARY_NAME} PRIVATE NDEBUG)
        target_compile_options(${LIBRARY_NAME} PRIVATE -O3)
        set_target_properties(
            ${LIBRARY_NAME}
            PROPERTIES
            SUFFIX ".bin"
        )
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Error configuring library! Not found sources dir: " "${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    target_include_directories(${LIBRARY_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    # Add library to added list
    set(
        ADDED_LIBRARIES_NOQT
        ${ADDED_LIBRARIES_NOQT}
        ${LIBRARY_NAME}
    )

    message(STATUS "Configured library " ${LIBRARY_NAME})
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

    # Runtime linking of target
    set_target_properties(
        ${LIBRARY_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH TRUE
        INSTALL_RPATH "./lib"
    )

    # Target linking
    target_link_libraries(
        ${LIBRARY_NAME}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Sql
    )

    # Build flag configuration
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${LIBRARY_NAME} PRIVATE DEBUG_MODE)
        target_compile_options(${LIBRARY_NAME} PRIVATE -g)
        set_target_properties(
            ${LIBRARY_NAME}
            PROPERTIES
            SUFFIX ".DEBUG"
        )
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${LIBRARY_NAME} PRIVATE NDEBUG)
        target_compile_options(${LIBRARY_NAME} PRIVATE -O3)
        set_target_properties(
            ${LIBRARY_NAME}
            PROPERTIES
            SUFFIX ".bin"
        )
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Error configuring library! Not found sources dir: " "${CMAKE_CURRENT_SOURCE_DIR}/src")
    endif()

    target_include_directories(${LIBRARY_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    install(TARGETS ${LIBRARY_NAME}
        RUNTIME     DESTINATION ${INSTALL_DESTINATION_PREFIX}
        LIBRARY     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib
        ARCHIVE     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib/static
        INCLUDES    DESTINATION ${INSTALL_DESTINATION_PREFIX}/include
    )

    # Add library to added list
    set(
        ADDED_LIBRARIES
        ${ADDED_LIBRARIES}
        ${LIBRARY_NAME}
    )

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



function(ADD_EDITOR_TEST EDITOR_TESTNAME)
    add_executable(${EDITOR_TESTNAME})

    file(GLOB TESTING_SOURCES "${EDITOR_TESTNAME}.cpp" ${RESOURCES_COMMONFILE})
    target_sources(${EDITOR_TESTNAME} PRIVATE ${TESTING_SOURCES})

    target_link_libraries(${EDITOR_TESTNAME} PRIVATE
        GTest::gtest_main
        GTest::gtest

        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
    )

    include(GoogleTest)
    gtest_discover_tests(${EDITOR_TESTNAME})

    # Add test to added list
    set(
        ADDED_EDITOR_TESTS
        ${ADDED_EDITOR_TESTS}
        ${EDITOR_TESTNAME}
    )
    message(STATUS "Added test: " ${EDITOR_TESTNAME})
endfunction()
