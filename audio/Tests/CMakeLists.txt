find_package(GTest REQUIRED)

file(
  GLOB_RECURSE
  PRT_AUDIO_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")

add_executable(prette-audio-tests main.cc ${PRT_AUDIO_TEST_SOURCES})
target_link_libraries(prette-audio-tests
  PUBLIC
    prette-audio
    GTest::gtest
    GTest::gmock
  PRIVATE)
target_include_directories(prette-audio-tests
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)

if(ENABLE_COVERAGE)
  create_coverage_target(prette-audio-tests)
endif()