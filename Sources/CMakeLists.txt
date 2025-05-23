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

list(
  APPEND
  PRT_LIBRARIES
  PNG::PNG
  JPEG::JPEG
  Freetype::Freetype
  libconfig::config
  units::units
  fmt::fmt
  jsonnet::jsonnet++
  tweeny
  rxcpp
  stduuid
  litehtml
  rapidjson
  harfbuzz::harfbuzz harfbuzz::harfbuzz-subset
  RocksDB::rocksdb RocksDB::rocksdb-shared
  glm::glm
  Backward::Backward
  OpenSSL::Crypto
  imgui::imgui
  Vulkan::Vulkan
  implot::implot
  unofficial::shaderc::shaderc
  OpenAL::OpenAL
  CpuFeatures::cpu_features
  ecl::ecl
  rapidjson
  yoga::yogacore
  re2::re2
  stduuid
  tinyobjloader::tinyobjloader
  draco::draco
  ${LUA_LIBRARIES}
  glfw
  libuv::uv)

if(PRT_TRACING_ENABLED)
  list(APPEND PRT_LIBRARIES Tracy::TracyClient)
endif()

add_compile_definitions(PRETTE_ENABLE_LUA)

add_library(${PROJECT_NAME} SHARED ${PRT_SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC ${PRT_COMPILE_OPTS} )
target_link_libraries(${PROJECT_NAME}
  PUBLIC ${PRT_LIBRARIES} prt-flatbuffers)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${LUA_INCLUDE_DIR} ${Stb_INCLUDE_DIR})

add_executable(${PROJECT_NAME}-client main.cc)
target_link_libraries(${PROJECT_NAME}-client PUBLIC ${PROJECT_NAME})

find_program(IWYU "include-what-you-use")
if(IWYU)
  message(STATUS "found iwyu: ${IWYU}")
  set(iwyu_options "-Xiwyu" "--mapping_file=${CMAKE_SOURCE_DIR}/iwyu.imp")
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${iwyu_options}")
  set_property(TARGET ${PROJECT_NAME}-client PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${iwyu_options}")
endif()