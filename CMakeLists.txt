cmake_minimum_required(VERSION 3.5)
project(prette)

include(GitInfo)
include(DoxygenConfig)
include(CppCheckConfig)

# cppcheck
find_package(Threads REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(glog REQUIRED)
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
#TODO: convert to vcpkg
find_package(utf8 REQUIRED)
find_package(OpenAL REQUIRED)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(PROJECT_GLFW_CONTEXT_VERSION_MAJOR 4)
set(PROJECT_GLFW_CONTEXT_VERSION_MINOR 1)

include_directories(${PROJECT_BINARY_DIR}/Sources/)

list(APPEND PRT_COMPILE_OPTS 
  -frtti
  -Wno-deprecated
  -Wno-sizeof-pointer-memaccess)

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
add_subdirectory(Tests)
add_subdirectory(Sources)
add_subdirectory(Benchmarks)