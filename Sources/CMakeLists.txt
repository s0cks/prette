configure_file(prette/prette.h.in ${CMAKE_CURRENT_BINARY_DIR}/prette/prette.h)
configure_file(prette/gfx.h.in ${CMAKE_CURRENT_BINARY_DIR}/prette/gfx.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(
  GLOB_RECURSE
  PRT_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc"
  "${CMAKE_CURRENT_BINARY_DIR}/prette/**/*.h")
find_path(TERMCOLOR_INCLUDE_DIRS "termcolor/termcolor.hpp")

list(
  APPEND
  PRT_LIBRARIES
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
  glfw
  Vorbis::vorbis
  libuv::uv_a)
if(PRT_ENABLE_VK)
  list(APPEND PRT_LIBRARIES Vulkan::Vulkan)
else()
  list(APPEND PRT_LIBRARIES OpenGL::GL GLEW::GLEW)
endif()

add_library(${PROJECT_NAME} STATIC ${PRT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${PRT_COMPILE_OPTS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PRT_LIBRARIES})
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${TERMCOLOR_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}-client main.cc)
target_link_libraries(${PROJECT_NAME}-client PUBLIC ${PROJECT_NAME})

if(ENABLE_COVERAGE)
  create_coverage_target(${PROJECT_NAME}-client)
endif()
