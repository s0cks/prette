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
  Freetype::Freetype
  libconfig::config
  units::units
  semver
  fmt::fmt
  jsonnet::jsonnet++
  tweeny
  rxcpp
  stduuid
  rapidjson
  RocksDB::rocksdb RocksDB::rocksdb-shared
  glm::glm
  Backward::Backward
  OpenSSL::Crypto
  imgui::imgui
  Vulkan::Vulkan
  implot::implot
  rapidjson
  yoga::yogacore
  stduuid
  absl::any absl::log absl::base absl::bits
  tinyobjloader::tinyobjloader
  ${LUA_LIBRARIES}
  glfw
  libuv::uv)

if(PRT_TRACING_ENABLED)
  list(APPEND PRT_LIBRARIES Tracy::TracyClient)
endif()

add_library(${PROJECT_NAME} SHARED ${PRT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${PRT_COMPILE_OPTS} )
target_link_libraries(${PROJECT_NAME}
  PUBLIC ${PRT_LIBRARIES} prt-flatbuffers)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${TERMCOLOR_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${Stb_INCLUDE_DIR})

add_executable(${PROJECT_NAME}-client main.cc)
target_link_libraries(${PROJECT_NAME}-client PUBLIC ${PROJECT_NAME})