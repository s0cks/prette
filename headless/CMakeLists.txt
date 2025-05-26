add_executable(prette-headless main.cc)
target_link_libraries(prette-headless
  PUBLIC prette-core prette-audio)
target_include_directories(prette-headless
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE)
set_property(TARGET prette-headless PROPERTY CXX_INCLUDE_WHAT_YOU_USE "${IWYU};;${IWYU_OPTS}")