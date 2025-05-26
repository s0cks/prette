find_package(flatbuffers CONFIG REQUIRED)

include(FlatC)
file(GLOB PRT_FLATBUFFER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.fbs)

add_flatc_target(flatbuffers ${PRT_FLATBUFFER_SOURCES})

file(GLOB PRT_FLATBUFFER_GENERATED_SOURCES
  "${FLATC_OUTPUT_DIR}/*.h"
  "${FLATC_OUTPUT_DIR}/*.cc")

add_library(prt-flatbuffers INTERFACE ${PRT_FLATBUFFER_GENERATED_SOURCES})
target_include_directories(prt-flatbuffers INTERFACE ${FLATC_BINARY_DIR}/Sources)
target_link_libraries(prt-flatbuffers INTERFACE flatbuffers::flatbuffers)