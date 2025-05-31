file(GLOB PRT_ASSETS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc")

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(draco CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

add_library(prt-assets SHARED ${PRT_ASSETS_SOURCES})
target_include_directories(prt-assets
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
target_link_libraries(prt-assets
  PUBLIC
    prette::stb
    prette::core #TODO: remove
    glm::glm
    JPEG::JPEG
    PNG::PNG
    Freetype::Freetype
    tinyobjloader::tinyobjloader
    draco::draco
  PRIVATE)

add_library(prette::assets ALIAS prt-assets)