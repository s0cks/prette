find_package(spirv_cross_reflect CONFIG REQUIRED)

file(GLOB PRT_SHADER_REFLECTOR_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_executable(prette-shader-reflector main.cc ${PRT_SHADER_REFLECTOR_SOURCES})
target_link_libraries(prette-shader-reflector
  PUBLIC
    prette::core
    spirv-cross-reflect
  PRIVATE)
target_include_directories(prette-shader-reflector
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
set_property(TARGET prette-stitcher PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")

add_executable(prette::shader-reflector ALIAS prette-shader-reflector)