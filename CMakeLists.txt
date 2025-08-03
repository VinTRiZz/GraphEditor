# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project configuration
project(
    GraphEditor
    VERSION 1.20.18
    LANGUAGES C CXX
)

# Versions
add_compile_definitions(
    GRAPH_EDITOR_VERSION="${PROJECT_VERSION}"
)

# Install prefix
set(
    INSTALL_DESTINATION_PREFIX
    "${CMAKE_CURRENT_SOURCE_DIR}/BIN/Package"
)

set(
    INSTALL_BINARY_NAME
    ${PROJECT_NAME}-${PROJECT_VERSION}
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


# Build configuration
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

# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Common things include (functions, etc.)
include(cmake/CMakeLists.txt)

# Subdirs
message(STATUS "Searching for libraries...")
add_subdirectory(Libraries)

message(STATUS "Searching for apps...")
add_subdirectory(App)

message(STATUS "Searching for tests...")
add_subdirectory(Testing EXCLUDE_FROM_ALL)

# ====================================================== #
# ETC CONFIGURATION

# ======================================== #
# Installation
install(TARGETS ${INSTALL_BINARY_NAME}
    RUNTIME     DESTINATION ${INSTALL_DESTINATION_PREFIX}
    LIBRARY     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib
    ARCHIVE     DESTINATION ${INSTALL_DESTINATION_PREFIX}/lib/static
    INCLUDES    DESTINATION ${INSTALL_DESTINATION_PREFIX}/include
)

set(
    GRAPH_EDITOR_DISABLE_EXECUTABLE ON
)

# Packet creation
include(InstallRequiredSystemLibraries)

set(
    GRAPH_EDITOR_INSTALL_APPIMAGE ON
)
set(CPACK_PACKAGE_NAME ${INSTALL_BINARY_NAME})
set(CPACK_PACKAGE_VERSION ${GRAPH_EDITOR_VERSION})

if (GRAPH_EDITOR_INSTALL_APPIMAGE)
    set(CPACK_GENERATOR "External")
    set(CPACK_EXTERNAL_ENABLED TRUE)
    set(CPACK_EXTERNAL_BUILT_PACKAGES "AppImage")
    set(CPACK_TOPLEVEL_TAG "AppImage")
    include(CPack)
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "VinTRiZz")
    set(CPACK_PACKAGE_DIRECTORY ${INSTALL_DESTINATION_PREFIX})
    include(CPack)
endif()
