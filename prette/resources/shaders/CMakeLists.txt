include(Vulkan)
add_shader_target(swapchain-shader ${CMAKE_CURRENT_SOURCE_DIR}/swapchain.vert ${CMAKE_CURRENT_SOURCE_DIR}/swapchain.frag)
add_shader_target(scene-shader ${CMAKE_CURRENT_SOURCE_DIR}/scene.vert ${CMAKE_CURRENT_SOURCE_DIR}/scene.frag)
add_shader_target(gui-shader ${CMAKE_CURRENT_SOURCE_DIR}/gui.vert ${CMAKE_CURRENT_SOURCE_DIR}/gui.frag)

add_custom_target(shaders ALL
  DEPENDS
    scene-shader
    swapchain-shader
    gui-shader
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})