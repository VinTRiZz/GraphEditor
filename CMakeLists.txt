# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project configuration
project(
    GraphEditor
    VERSION 2.0.0
    LANGUAGES C CXX
)

# Versions
add_compile_definitions(
    GRAPH_EDITOR_VERSION="${PROJECT_VERSION}"
)

# Install prefix
set(
    INSTALL_DESTINATION_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/BIN"
)

# Defines for components libs
set(COMPONENTS_COMPONENT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Components )
set(COMPONENTS_INSTALL_PATH ${INSTALL_DESTINATION_PREFIX} )
set(COMPONENTS_IS_ENABLED_QT ON )

# Qt
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)

# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Common things include (functions, etc.)
include(cmake/CMakeLists.txt)

message(STATUS "Searching for GTest")
add_subdirectory(Testing EXCLUDE_FROM_ALL)

# Subdirs
message(STATUS "Searching for libraries...")
add_subdirectory(Libraries)

# Subdirs
message(STATUS "Searching for plugins...")
add_subdirectory(Plugins)

message(STATUS "Searching for apps...")
add_subdirectory(App)

