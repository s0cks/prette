find_package(GTest REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE MCC_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")

add_executable(${PROJECT_NAME}-tests
  main.cc
  ${MCC_TEST_SOURCES})
target_compile_options(${PROJECT_NAME}-tests
  PRIVATE ${MCC_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME}-tests
  ${PROJECT_NAME}
  GTest::gtest
  GTest::gmock)