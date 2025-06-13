find_package(flatbuffers CONFIG REQUIRED)

include(FlatC)

add_flatc_target(flatc-asset "${CMAKE_CURRENT_SOURCE_DIR}/asset.fbs")
add_flatc_target(flatc-chunk "${CMAKE_CURRENT_SOURCE_DIR}/chunk.fbs")

file(GLOB PRT_FLATBUFFER_GENERATED_SOURCES
  "${FLATC_OUTPUT_DIR}/*.h"
  "${FLATC_OUTPUT_DIR}/*.cc")

add_library(prette-flatbuffers
  INTERFACE ${PRT_FLATBUFFER_GENERATED_SOURCES})
target_include_directories(prette-flatbuffers
  INTERFACE ${FLATC_BINARY_DIR}/Sources)
target_link_libraries(prette-flatbuffers
  INTERFACE flatbuffers::flatbuffers)
add_dependencies(prette-flatbuffers
  flatc-asset
  flatc-chunk)

add_library(prette::flatbuffers ALIAS prette-flatbuffers)