include(Vulkan)
add_shader_target(test-shader ${CMAKE_CURRENT_SOURCE_DIR}/test.vert ${CMAKE_CURRENT_SOURCE_DIR}/test.frag)
add_shader_target(gui-shader ${CMAKE_CURRENT_SOURCE_DIR}/gui.vert ${CMAKE_CURRENT_SOURCE_DIR}/gui.frag)

add_custom_target(shaders ALL
  DEPENDS
    test-shader
    gui-shader
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})