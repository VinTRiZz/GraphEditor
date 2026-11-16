cmake_minimum_required(VERSION 3.24) # for checking that linking is possible

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subdirs
add_subdirectory(functions)
add_subdirectory(testing EXCLUDE_FROM_ALL)
