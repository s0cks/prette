include(Vulkan)
add_glslc_target(test-vertex ${CMAKE_CURRENT_SOURCE_DIR}/test.vert)
add_glslc_target(test-fragment ${CMAKE_CURRENT_SOURCE_DIR}/test.frag)

add_custom_target(shaders ALL
  DEPENDS
    test-vertex
    test-fragment
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})