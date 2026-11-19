# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    GraphEditor
    VERSION 3.02.0000
    LANGUAGES C CXX
)


# Installation
set(INSTALL_DESTINATION_PREFIX
    "${CMAKE_CURRENT_LIST_DIR}/BIN"
)
set(CMAKE_INSTALL_PREFIX
    "${INSTALL_DESTINATION_PREFIX}"
)

add_subdirectory(ComponentsSystem)

find_package(boost_system REQUIRED)
find_package(boost_thread REQUIRED)
COMPONENTS_CCR_DEFINE_PROJECT(${PROJECT_NAME} ${PROJECT_VERSION} 3rdparty Libraries Apps)
