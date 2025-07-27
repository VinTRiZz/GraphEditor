# Project files
file(GLOB HEADERS "src/*.hpp" "src/*.h")
file(GLOB SOURCES "src/*.cpp")
file(GLOB UIFILES "src/*.ui")
set(
    PROJECT_FILES
    ${HEADERS} ${SOURCES} ${UIFILES} ${RESOURCES_COMMONFILE}
)

set(
    PROJECT_LINK_LIBS
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    -lboost_system
    -lboost_program_options
)

# Main target
add_executable( ${PROJECT_NAME} )
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_FILES})
target_link_libraries( ${PROJECT_NAME} PRIVATE ${PROJECT_LINK_LIBS} )
LINK_TARGET_LIBRARY(${PROJECT_NAME} Common)
LINK_TARGET_LIBRARY(${PROJECT_NAME} EditorForm)
CONFIGURE_SUFFIX(${PROJECT_NAME} ".bin.DEBUG" ".bin")
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH "./lib"
)

add_executable( ${INSTALL_BINARY_NAME} )
target_sources(${INSTALL_BINARY_NAME} PRIVATE ${PROJECT_FILES})
target_link_libraries( ${INSTALL_BINARY_NAME} PRIVATE ${PROJECT_LINK_LIBS} )
LINK_TARGET_LIBRARY(${INSTALL_BINARY_NAME} Common)
LINK_TARGET_LIBRARY(${INSTALL_BINARY_NAME} EditorForm)
target_compile_definitions(${INSTALL_BINARY_NAME} PRIVATE NDEBUG)
target_compile_options(${INSTALL_BINARY_NAME} PRIVATE -O3)
set_target_properties(${INSTALL_BINARY_NAME} PROPERTIES SUFFIX ".bin")
set_target_properties(
    ${INSTALL_BINARY_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH "../lib"
)
