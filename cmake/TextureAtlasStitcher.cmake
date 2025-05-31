function(add_texture_atlas_target atlas_name atlas_width atlas_height)
  set(ATLAS_ENCODING "png")

  set(atlas_texture_file "${CMAKE_CURRENT_SOURCE_DIR}/${atlas_name}.${ATLAS_ENCODING}")
  set(atlas_manifest_file "${CMAKE_CURRENT_SOURCE_DIR}/${atlas_name}.json")
  set(atlas_manifest_header_file "${CMAKE_CURRENT_BINARY_DIR}/prette/${atlas_name}.h")

  list(APPEND stitcher_opts
    --atlas_name=${atlas_name}
    --atlas_width=${atlas_width}
    --atlas_height=${atlas_height}
    --out_header_filename=${atlas_manifest_header_file})

  add_custom_command(
    OUTPUT ${atlas_manifest_header_file} ${atlas_texture_file}
    COMMAND
      $<TARGET_FILE:prette::stitcher> ${stitcher_opts}
    COMMENT "stitching ${atlas_name} texture atlas....."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  add_custom_target(${atlas_name} DEPENDS ${atlas_manifest_header_file} ${atlas_texture_file})
  set(${atlas_name}_SOURCES ${atlas_manifest_header_file} PARENT_SCOPE)
endfunction()