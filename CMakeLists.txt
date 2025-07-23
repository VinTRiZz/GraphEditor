# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project configuration
project(
    GraphEditor
    VERSION 0.9.5
    LANGUAGES C CXX
)

add_compile_definitions(
    GRAPH_EDITOR_VERSION="${PROJECT_VERSION}"
)

# For LTO
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

# Link time optimization (for future)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
   message(STATUS "LTO enabled")
else()
   message(WARNING "LTO not supported: ${ipo_error}")
endif()

# Build binary directory
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN
)

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN/lib
)

set(
    CMAKE_CACHEFILE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/build
)

# Qt
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Sql)

# Common things include (functions, etc.)
include(cmake/CMakeLists.txt)

# Subdirs
message(STATUS "Searching for libraries...")
add_subdirectory(Libraries)

message(STATUS "Searching for apps...")
add_subdirectory(App)

message(STATUS "Searching for tests...")
add_subdirectory(Testing)

add_custom_target(
    APPLICATION_WITH_TESTS DEPENDS
    ALL_LIBRARIES
    ${PROJECT_NAME}
    ${ADDED_EDITOR_TESTS}
)
