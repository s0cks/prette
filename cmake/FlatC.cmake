find_program(FLATC NAMES "flatc")
if(NOT FLATC)
  message(FATAL "failed to find flatc")
endif()

execute_process(
  COMMAND ${FLATC} --version
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  ERROR_QUIET
  OUTPUT_VARIABLE FLATC_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" FLATC_VERSION "${FLATC_VERSION}")

message(STATUS "found flatc v${FLATC_VERSION}: ${FLATC}")

set(FLATC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(FLATC_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(FLATC_OUTPUT_DIR "${FLATC_BINARY_DIR}/Sources/prette")

set(FLATC_TARGET "--cpp")
set(FLATC_OPTS ${FLATC_TARGET} -o ${FLATC_OUTPUT_DIR} -I ${FLATC_SOURCE_DIR})

message(STATUS "flatc options: ${FLATC_OPTS}")
function(add_flatc_target target_name target_source)
  add_custom_target(
    ${target_name} ALL
    COMMENT "Compiling ${target_name}"
    SOURCES
      ${target_source}
    WORKING_DIRECTORY ${FLATC_SOURCE_DIR}
    VERBATIM
    COMMAND
      ${FLATC} ${FLATC_OPTS} ${target_source}
  )
endfunction()

function(add_schema_library lib_name schema_sources)
  foreach(schema_source IN LISTS schema_sources)
    get_filename_component(target_name ${schema_source} NAME_WLE)
    set(target_name "${target_name}-schema")
    list(APPEND lib_targets ${target_name})
    add_flatc_target(${target_name} ${schema_source})
  endforeach()
  file(GLOB generated_sources
    "${FLATC_OUTPUT_DIR}/*.h"
    "${FLATC_OUTPUT_DIR}/*.cc")

  add_library(${lib_name}
    INTERFACE ${generated_sources})
  target_include_directories(${lib_name}
    INTERFACE ${FLATC_BINARY_DIR}/Sources)
  target_link_libraries(${lib_name}
    INTERFACE flatbuffers::flatbuffers)
  add_dependencies(${lib_name} ${lib_targets})
endfunction()