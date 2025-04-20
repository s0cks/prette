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
function(add_flatc_target target_name target_sources)
  add_custom_target(
    ${target_name} ALL
    COMMENT "Compiling flatbuffers"
    SOURCES
      ${target_sources}
    WORKING_DIRECTORY ${FLATC_SOURCE_DIR}
    VERBATIM
    COMMAND
      ${FLATC} ${FLATC_OPTS} ${target_sources}
  )
endfunction()