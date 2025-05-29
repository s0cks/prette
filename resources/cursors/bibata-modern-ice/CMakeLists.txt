add_cursor(PRT_CURSOR_LEFT_PTR left_ptr.png)
add_cursor(PRT_CURSOR_RIGHT_PTR right_ptr.png)
add_cursor(PRT_CURSOR_HAND1 hand1.png)
add_cursor(PRT_CURSOR_HAND2 hand2.png)
add_cursor(PRT_CURSOR_GRABBING grabbing.png)
add_cursor(PRT_CURSOR_MOVE move.png)
add_cursor(PRT_CURSOR_ZOOM_IN zoom-out.png)
add_cursor(PRT_CURSOR_ZOOM_OUT zoom-in.png)

set(PRT_CURSOR_NAME)
set(PRT_CURSOR_NAME "bibata-modern-ice")
configure_file(bibata_modern_ice.h.in ${CMAKE_CURRENT_BINARY_DIR}/prette/bibata_modern_ice.h)

add_library(prt-cursor-bibata-modern-ice
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/prette/bibata_modern_ice.h)
target_include_directories(prt-cursor-bibata-modern-ice
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_library(prette::cursor-bibata-modern-ice ALIAS prt-cursor-bibata-modern-ice)