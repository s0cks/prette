enable_testing()

find_package(GTest REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PRT_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")

add_executable(${PROJECT_NAME}-tests
  main.cc
  ${PRT_TEST_SOURCES})
target_compile_options(${PROJECT_NAME}-tests
  PRIVATE ${PRT_COMPILE_OPTS})
target_link_libraries(${PROJECT_NAME}-tests
  ${PROJECT_NAME}
  GTest::gtest
  GTest::gmock)