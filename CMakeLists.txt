cmake_minimum_required(VERSION 3.5)
project(prette CXX)

include(BuildInfo)

option(ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
if(ENABLE_CLANG_TIDY AND ${CMAKE_BUILD_TYPE} MATCHES "^[rR]elease")
  include(ClangTidy)
endif()

option(PRT_BUILD_DOCS "Build documentation" ON)
if(PRT_BUILD_DOCS)
  include(Doxygen)
endif()

option(ENABLE_CPPCHECK "Enable CppCheck" ON)
if(ENABLE_CPPCHECK AND ${CMAKE_BUILD_TYPE} MATCHES "^[rR]elease")
  include(CppCheck)
endif()

set(PRT_ENABLE_VK 1)
if(PRT_ENABLE_VK)
  find_package(Vulkan REQUIRED)
  add_compile_definitions(PRT_VK)
else()
  find_package(OpenGL REQUIRED COMPONENTS OpenGL)
  find_package(GLEW REQUIRED)
  add_compile_definitions(PRT_GL)
  set(PROJECT_GLFW_CONTEXT_VERSION_MAJOR 4)
  set(PROJECT_GLFW_CONTEXT_VERSION_MINOR 1)
endif()

# cppcheck
find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(rxcpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(leveldb CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Backward CONFIG REQUIRED)
find_package(units CONFIG REQUIRED)
find_package(tweeny CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)
set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(unofficial-nuklear CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)

find_path(SEMVER_INCLUDE_DIRS "semver/semver.hpp")

#TODO: convert to vcpkg
find_package(utf8 REQUIRED)
find_package(OpenAL REQUIRED)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

include_directories(${PROJECT_BINARY_DIR}/Sources/)

list(APPEND PRT_COMPILE_OPTS
  -frtti
  -fms-extensions
  -Wno-deprecated
  -Wno-sizeof-pointer-memaccess)

option(ENABLE_COVERAGE "Enable code coverage reports using gcov/lcov" ON)
if(ENABLE_COVERAGE)
  include(Coverage)
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "([aA]pple)?[cC]lang|GNU")
    list(APPEND PRT_COMPILE_OPTS
        -O0
        -g
        -fprofile-instr-generate
        -fcoverage-mapping
        --coverage)
    list(APPEND PRT_LIBRARIES
      -fprofile-instr-generate
      -fprofile-instr-generate
      -fcoverage-mapping
      --coverage)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #TODO: implement
  else()
    message(FATAL_ERROR "unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
endif()

add_compile_definitions(PRT_GLFW)
if(CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
  list(APPEND PRT_COMPILE_OPTS "-Wno-exceptions")
  add_compile_definitions(PRT_DEBUG)
  add_compile_definitions(PTR_VMEMPROF)
endif()

link_libraries(Threads::Threads
               glog::glog
               gflags::gflags)

add_subdirectory(lib/semver)
add_subdirectory(Sources)

option(PRT_BUILD_BENCHMARKS "Build the benchmarks" ON)
if(PRT_BUILD_BENCHMARKS)
  add_subdirectory(Benchmarks)
endif()

option(PRT_BUILD_TESTS "Build the tests" ON)
if(PRT_BUILD_TESTS)
  add_subdirectory(Tests)
endif()