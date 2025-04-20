find_program(GLSLC NAMES "glslc")
if(NOT GLSLC)
  message(FATAL "failed to find glslc")
endif()

execute_process(
  COMMAND ${GLSLC} --version
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  ERROR_QUIET
  OUTPUT_VARIABLE GLSLC_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" GLSLC_VERSION "${GLSLC_VERSION}")
message(STATUS "found glslc v${GLSLC_VERSION}: ${GLSLC}")

set(GLSLC_OPTS)
list(APPEND GLSLC_OPTS "--target-env=vulkan1.2")
list(APPEND GLSLC_OPTS "--target-spv=spv1.5")

function(add_glslc_target target_name source)
  get_filename_component(shader_filename ${source} NAME)
  set(shader_out ${CMAKE_CURRENT_BINARY_DIR}/${shader_filename}.spv)
  add_custom_target(
    ${target_name} ALL
    COMMENT "Compiling ${shader_filename} shader"
    SOURCES
      ${source}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS
      ${shader_out}
    VERBATIM
    COMMAND
      ${GLSLC} ${GLSLC_OPTS} ${source} -o ${shader_out})
endfunction()

function(add_shader_target shader_name vertex_shader_source fragment_shader_source)
  add_glslc_target(${shader_name}-vert ${vertex_shader_source})
  add_glslc_target(${shader_name}-frag ${fragment_shader_source})
  add_custom_target(${shader_name} ALL
    DEPENDS
      ${shader_name}-vert
      ${shader_name}-frag
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()