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
  Backward::Backward
  OpenSSL::Crypto
  imgui::imgui
  implot::implot
  rapidjson
  stduuid
  absl::any absl::log absl::base absl::bits
  tinyobjloader::tinyobjloader
  ${LUA_LIBRARIES}
  glfw
  libuv::uv)
if(PRT_ENABLE_VK)
  list(APPEND PRT_LIBRARIES Vulkan::Vulkan)
else()
  list(APPEND PRT_LIBRARIES OpenGL::GL GLEW::GLEW)
endif()

if(PRT_TRACING_ENABLED)
  list(APPEND PRT_LIBRARIES Tracy::TracyClient)
endif()

add_library(${PROJECT_NAME} SHARED ${PRT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${PRT_COMPILE_OPTS})
target_link_libraries(${PROJECT_NAME}
  PUBLIC ${PRT_LIBRARIES} prt-flatbuffers)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${TERMCOLOR_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${Stb_INCLUDE_DIR})

add_executable(${PROJECT_NAME}-client main.cc)
target_link_libraries(${PROJECT_NAME}-client PUBLIC ${PROJECT_NAME})