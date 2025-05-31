include(TextureAtlasStitcher)

set(ATLAS1_WIDTH 4096)
set(ATLAS1_HEIGHT 2048)
add_texture_atlas_target(atlas1 ${ATLAS1_WIDTH} ${ATLAS1_HEIGHT})

add_library(prt-atlas1 INTERFACE ${atlas1_SOURCES})
target_include_directories(prt-atlas1
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_library(prette::atlas1 ALIAS prt-atlas1)