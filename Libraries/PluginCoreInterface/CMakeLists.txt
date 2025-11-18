set(
    LIBRARY_NAME
    PluginCoreInterface
)

# Files
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE UIFILES "src/*.ui")
source_group("Ui Files" FILES ${UIFILES})

# Library target
add_library(${LIBRARY_NAME} INTERFACE)
target_sources(${LIBRARY_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES})
target_include_directories(${LIBRARY_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set(
    TARGET_LINKED_LIBS
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)

message(STATUS "Configured plugin core interface")
