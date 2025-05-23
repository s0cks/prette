cmake_minimum_required(VERSION 3.5)

include(BuildInfo)
project(
  ${BUILD_JSON_NAME}
  VERSION ${BUILD_JSON_VERSION}
  HOMEPAGE_URL ${BUILD_JSON_HOMEPAGE}
  LANGUAGES C CXX)

option(ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
if(ENABLE_CLANG_TIDY)
  include(ClangTidy)
endif()

option(PRT_BUILD_DOCS "Build documentation" ON)
if(PRT_BUILD_DOCS)
  include(Doxygen)
endif()

include(CCache)
include(Mold)
include(Bloaty)

include(Vulkan)
find_package(Vulkan REQUIRED)

find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(rxcpp CONFIG REQUIRED)
find_package(Jsonnet REQUIRED)
find_package(ECL REQUIRED)
find_package(draco CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)

find_package(OpenAL REQUIRED)

set(OPENSSL_USE_STATIC_LIBS FALSE)
if(APPLE)
  find_package(OpenSSL PATHS /opt/homebrew REQUIRED)
else()
  find_package(OpenSSL REQUIRED)
endif()

find_package(fmt CONFIG REQUIRED)
find_package(Backward CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)
find_package(tweeny CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)
set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(libconfig CONFIG REQUIRED)
find_package(yoga CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(CpuFeatures CONFIG REQUIRED)
find_package(litehtml CONFIG REQUIRED)

list(APPEND PRT_COMPILE_OPTS -frtti -fms-extensions -Wno-deprecated
     -Wno-sizeof-pointer-memaccess)

# option(ENABLE_COVERAGE "Enable code coverage reports using gcov/lcov" OFF)
# if(ENABLE_COVERAGE)
#   include(Coverage)
#   # TODO: cleanup this logic, get it into Coverage.cmake
#   if(${CMAKE_CXX_COMPILER_ID} MATCHES "([aA]pple)?[cC]lang|GNU")
#     list(
#       APPEND
#       PRT_COMPILE_OPTS
#       -O0
#       -g
#       -fprofile-instr-generate
#       -fcoverage-mapping
#       --coverage)
#     list(APPEND PRT_LIBRARIES -fprofile-instr-generate -fprofile-instr-generate
#          -fcoverage-mapping --coverage)
#   elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#     # TODO: implement
#   else()
#     message(FATAL_ERROR "unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
#   endif()
# endif()

add_compile_definitions(PRT_GLFW)
if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  list(APPEND PRT_COMPILE_OPTS "-Wno-exceptions")
  add_compile_definitions(PRT_DEBUG)
  add_compile_definitions(PTR_VMEMPROF)
endif()

option(PRT_TRACING_ENABLED "" OFF)
if(PRT_TRACING_ENABLED)
  find_package(Tracy CONFIG REQUIRED)
  add_compile_definitions(PRT_TRACING)
endif()

link_libraries(Threads::Threads glog::glog gflags::gflags)

add_subdirectory(prette)
add_subdirectory(Sources)

option(PRT_BUILD_BENCHMARKS "Build the benchmarks" ON)
if(PRT_BUILD_BENCHMARKS)
  add_subdirectory(Benchmarks)
endif()

option(PRT_BUILD_TESTS "Build the tests" ON)
if(PRT_BUILD_TESTS)
  add_subdirectory(Tests)
endif()
