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
include(IWYU)

list(APPEND PRT_COMPILE_OPTS
  -frtti
  -fms-extensions
  -Wno-deprecated
  -Wno-exceptions
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

option(PRT_ENABLE_LUA "Build with lua scripting." ON)
if(PRT_ENABLE_LUA)
  add_compile_definitions(PRT_ENABLE_LUA)
endif()

add_compile_definitions(PRT_GLFW)
if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  add_compile_definitions(PRT_DEBUG)
  add_compile_definitions(PTR_VMEMPROF)
endif()

option(PRT_TRACING_ENABLED "Build with tracing integration enabled (tracy)" OFF)
if(PRT_TRACING_ENABLED)
  find_package(Tracy CONFIG REQUIRED)
  add_compile_definitions(PRT_ENABLE_TRACING)
endif()

option(PRT_BUILD_TESTS "Build tests" ON)
if(PRT_BUILD_TESTS)
  enable_testing()
endif()

option(PRT_BUILD_BENCHMARKS "Build benchmarks" ON)
if(PRT_BUILD_BENCHMARKS)
  add_subdirectory(Benchmarks)
endif()

add_subdirectory(core)
add_subdirectory(renderer)
add_subdirectory(audio)
add_subdirectory(sandbox)
add_subdirectory(headless)

add_subdirectory(resources)
configure_file(prette.cfg.in ${CMAKE_CURRENT_SOURCE_DIR}/prette.cfg)