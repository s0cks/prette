configure_file(prette/prette.h.in ${PROJECT_BINARY_DIR}/Sources/prette/prette.h)
configure_file(prette/gfx.h.in ${PROJECT_BINARY_DIR}/Sources/prette/gfx.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PRT_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")
add_library(${PROJECT_NAME}
  ${PROJECT_BINARY_DIR}/Sources/prette/prette.h prette/prette.cc
  ${PROJECT_BINARY_DIR}/Sources/prette/gfx.h
  ${PRT_SOURCES})
target_link_libraries(${PROJECT_NAME}
  PNG::PNG
  JPEG::JPEG
  leveldb::leveldb
  flatbuffers::flatbuffers
  Freetype::Freetype
  units::units
  semver
  fmt::fmt
  tweeny
  rxcpp
  stduuid
  rapidjson
  nuklear
  glm::glm
  OpenAL::OpenAL
  Backward::Backward
  OpenSSL::Crypto
  rapidjson
  Vorbis::vorbis
  libuv::uv_a)
target_compile_options(${PROJECT_NAME}
  PRIVATE ${PRT_COMPILE_OPTS})
target_include_directories(${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/Sources ${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME}-client
  main.cc)
target_link_libraries(${PROJECT_NAME}-client
  ${PROJECT_NAME}
  OpenGL::GL
  glfw
  GLEW::GLEW)