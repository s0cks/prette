find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(Crc32c CONFIG REQUIRED)

file(GLOB PRT_TEXTURE_STITCHER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

add_executable(prette-stitcher main.cc ${PRT_TEXTURE_STITCHER_SOURCES})
target_link_libraries(prette-stitcher
  PUBLIC
    prette::assets
    Crc32c::crc32c
    Vulkan::Vulkan
    glfw
    pantor::inja
  PRIVATE)
target_include_directories(prette-stitcher
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${Stb_INCLUDE_DIR})
set_property(TARGET prette-stitcher PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")

add_executable(prette::stitcher ALIAS prette-stitcher)