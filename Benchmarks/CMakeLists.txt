find_package(benchmark REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PRT_BENCHMARK_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc")

add_executable(${CMAKE_PROJECT_NAME}-benchmarks
  main.cc
  ${PRT_BENCHMARK_SOURCES})
target_compile_options(${PROJECT_NAME}-benchmarks
  PRIVATE ${PRT_COMPILE_OPTS})
target_link_libraries(${CMAKE_PROJECT_NAME}-benchmarks
  ${PROJECT_NAME}
  benchmark::benchmark)