# Project files
file(GLOB HEADERS "src/*.hpp" "src/*.h")
file(GLOB SOURCES "src/*.cpp")
file(GLOB UIFILES "src/*.ui")

source_group("Ui Files" FILES ${UIFILES})

# Main target
add_executable( ${PROJECT_NAME} )

target_sources(${PROJECT_NAME} PRIVATE
    ${HEADERS}
    ${SOURCES}
    ${UIFILES}
    ${RESOURCES_COMMONFILE}
)

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

# Link to required libraries
LINK_TARGET_LIBRARY(${PROJECT_NAME} Common)
LINK_TARGET_LIBRARY(${PROJECT_NAME} EditorForm)


# ====================================================== #
# ETC CONFIGURATION

# Build flag configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE_MODE)
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
