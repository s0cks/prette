configure_file(prette/prette.h.in ${CMAKE_CURRENT_BINARY_DIR}/prette/prette.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Threads REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(rxcpp CONFIG REQUIRED)
find_package(Jsonnet REQUIRED)
find_package(ECL REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(marl CONFIG REQUIRED)
find_package(Backward CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)
set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(libconfig CONFIG REQUIRED)
find_package(CpuFeatures CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

set(OPENSSL_USE_STATIC_LIBS FALSE)
if(APPLE)
  find_package(OpenSSL PATHS /opt/homebrew REQUIRED)
else()
  find_package(OpenSSL REQUIRED)
endif()

file(
  GLOB_RECURSE
  PRT_CORE_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc"
  "${CMAKE_CURRENT_BINARY_DIR}/prette/**/*.h")

list(APPEND PRT_CORE_LIBRARIES
  prt-flatbuffers
  Threads::Threads
  glog::glog
  gflags::gflags
  libconfig::config
  marl::marl
  units::units
  fmt::fmt
  jsonnet::jsonnet++
  rxcpp
  stduuid
  rapidjson
  RocksDB::rocksdb RocksDB::rocksdb-shared
  Backward::Backward
  OpenSSL::Crypto
  CpuFeatures::cpu_features
  ecl::ecl
  rapidjson
  re2::re2
  stduuid
  ${LUA_LIBRARIES}
  libuv::uv)

if(PRT_TRACING_ENABLED)
  list(APPEND PRT_CORE_LIBRARIES Tracy::TracyClient)
endif()

add_library(prette-core SHARED ${PRT_CORE_SOURCES})
target_compile_options(prette-core
  PUBLIC ${PRT_COMPILE_OPTS}
  PRIVATE)
target_link_libraries(prette-core
  PUBLIC ${PRT_CORE_LIBRARIES})
target_include_directories(prette-core
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${LUA_INCLUDE_DIR})
set_property(TARGET prette-core PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")
add_dependencies(prette-core
  prt-flatbuffers)

add_library(prette::core ALIAS prette-core)