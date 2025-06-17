find_package(flatbuffers CONFIG REQUIRED)

include(FlatC)

file(GLOB PRT_FLATBUFFER_SCHEMA_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.fbs")

add_schema_library(prt-schema "${PRT_FLATBUFFER_SCHEMA_SOURCES}")
add_library(prette::schema ALIAS prt-schema)