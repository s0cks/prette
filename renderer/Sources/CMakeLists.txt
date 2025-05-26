include(Vulkan)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(draco CONFIG REQUIRED)
find_package(OpenAL REQUIRED)
find_package(tweeny CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(yoga CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(harfbuzz CONFIG REQUIRED)
find_package(litehtml CONFIG REQUIRED)

configure_file(prette/gfx.h.in ${CMAKE_CURRENT_BINARY_DIR}/prette/gfx.h)

file(
  GLOB_RECURSE
  PRT_RENDERER_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/prette/**/*.cc"
  "${CMAKE_CURRENT_BINARY_DIR}/prette/**/*.h")

list(APPEND
  PRT_RENDERER_LIBRARIES
  PNG::PNG
  Freetype::Freetype
  JPEG::JPEG
  tweeny
  litehtml
  harfbuzz::harfbuzz harfbuzz::harfbuzz-subset
  imgui::imgui
  Vulkan::Vulkan
  implot::implot
  unofficial::shaderc::shaderc
  yoga::yogacore
  tinyobjloader::tinyobjloader
  draco::draco
  glfw)

add_library(prette-renderer SHARED ${PRT_RENDERER_SOURCES})
target_link_libraries(prette-renderer
  PUBLIC prette-core ${PRT_RENDERER_LIBRARIES}
  PRIVATE)
target_include_directories(prette-renderer
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE)
set_property(TARGET prette-renderer PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")

