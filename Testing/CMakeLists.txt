set(
    TESTING_GTEST_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest_v1.17.x
)

if(NOT EXISTS ${TESTING_GTEST_DIR})
    message(FATAL_ERROR "Error searching for GTest: directory ${TESTING_GTEST_DIR} not exist")
endif()

enable_testing()
add_subdirectory(${TESTING_GTEST_DIR})
