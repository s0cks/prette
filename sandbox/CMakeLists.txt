add_executable(prette-sandbox main.cc)
# add_dependencies(prette-sandbox
#   shaders)
target_link_libraries(prette-sandbox
  PUBLIC
    prette-core
    prette-renderer
    prette-audio
  PRIVATE)
target_include_directories(prette-sandbox
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
set_property(TARGET prette-sandbox PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")
