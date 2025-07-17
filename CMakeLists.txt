# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Project configuration
project(
    GraphEditor
    VERSION 1
    LANGUAGES C CXX
)

# For LTO
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

# Link time optimization (for future)
#include(CheckIPOSupported)
#check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
#if(ipo_supported)
#    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#    message(STATUS "LTO enabled")
#else()
#    message(WARNING "LTO not supported: ${ipo_error}")
#endif()


# Build binary directory
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN
)

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN
)

set(
    CMAKE_CACHEFILE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/build
)


# Project files
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE UIFILES "src/*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

source_group("Ui Files" FILES ${UIFILES})


# Library searches
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Sql)


# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Target
add_executable( ${PROJECT_NAME} )

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CACHEFILE_DIR} )
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src )

# Resources
qt_add_resources(RCC_SOURCES ${RESOURCES})
target_sources(${PROJECT_NAME} PRIVATE ${RCC_SOURCES})

target_sources(${PROJECT_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES})

# Precompiled headers (for future)
#file(GLOB PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} "src/Graph/*.h")
#file(GLOB PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} "src/GUI/CustomWidgets/*.h")
#file(GLOB PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} "src/GUI/ObjectScene/*.h")
#file(GLOB PRECOMPILED_HEADERS ${PRECOMPILED_HEADERS} "src/GUI/ObjectScene/PredefinedObjects/*.h")
#target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILED_HEADERS})


# Runtime linking of target
set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
    INSTALL_RPATH "./lib"
)

# Target linking
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    -lboost_system
    -lboost_program_options
)

# Build flag configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()

# Build suffixes
if (NOT RELEASE)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        SUFFIX ".DEBUG"
    )
else()
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        SUFFIX ".bin"
    )
endif()
