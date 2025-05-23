include(Vulkan)
add_shader_target(simple_textured2d ${CMAKE_CURRENT_SOURCE_DIR}/simple_texture2d.vert ${CMAKE_CURRENT_SOURCE_DIR}/simple_texture2d.frag)
add_shader_target(simple-colored-2d ${CMAKE_CURRENT_SOURCE_DIR}/simple_colored_2d.vert ${CMAKE_CURRENT_SOURCE_DIR}/simple_colored_2d.frag)
add_shader_target(scene-shader ${CMAKE_CURRENT_SOURCE_DIR}/scene.vert ${CMAKE_CURRENT_SOURCE_DIR}/scene.frag)
add_shader_target(gui-shader ${CMAKE_CURRENT_SOURCE_DIR}/gui.vert ${CMAKE_CURRENT_SOURCE_DIR}/gui.frag)
add_shader_target(tile-shader ${CMAKE_CURRENT_SOURCE_DIR}/tile.vert ${CMAKE_CURRENT_SOURCE_DIR}/tile.frag)
add_shader_target(grid-shader ${CMAKE_CURRENT_SOURCE_DIR}/grid.vert ${CMAKE_CURRENT_SOURCE_DIR}/grid.frag)

add_custom_target(shaders ALL
  DEPENDS
    scene-shader
    simple-colored-2d
    simple_textured2d
    gui-shader
    tile-shader
    grid-shader
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})