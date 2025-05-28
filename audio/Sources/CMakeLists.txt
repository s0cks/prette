find_package(OpenAL REQUIRED)
find_package(wave REQUIRED)

file(
  GLOB_RECURSE
  PRT_AUDIO_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc"
  "${CMAKE_CURRENT_BINARY_DIR}/prette/**/*.h")

list(APPEND
  PRT_AUDIO_LIBRARIES
  wave::wave
  OpenAL::OpenAL)

add_library(prette-audio SHARED ${PRT_AUDIO_SOURCES})
target_link_libraries(prette-audio
  PUBLIC prette-core ${PRT_AUDIO_LIBRARIES}
  PRIVATE)
target_include_directories(prette-audio
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
set_property(TARGET prette-audio PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")

add_executable(prette-audio-example main.cc)
target_link_libraries(prette-audio-example
  PUBLIC prette-audio
  PRIVATE)
target_include_directories(prette-audio-example
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
set_property(TARGET prette-audio-example PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")