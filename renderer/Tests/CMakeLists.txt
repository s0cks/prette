find_package(GTest REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(
  GLOB_RECURSE
  PRT_RENDERER_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")

add_executable(renderer-tests main.cc ${PRT_RENDERER_TEST_SOURCES})
target_link_libraries(renderer-tests
  PUBLIC
    prette-core
    GTest::gtest
    GTest::gmock
  PRIVATE)
target_include_directories(renderer-tests
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)

if(ENABLE_COVERAGE)
  create_coverage_target(renderer-tests)
endif()