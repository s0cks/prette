#include "prette/cursor.h"
#include "prette/gfx.h"
#ifdef PRT_GLFW

#include <GLFW/glfw3.h>
#include <cstdint>

namespace prt {
auto NewCursor(const uint8_t* bytes, const int width, const int height) -> GLFWcursor* {
  GLFWimage image;
  image.width = width;
  image.height = height;
  image.pixels = (uint8_t*)bytes;  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  const auto cursor = glfwCreateCursor(&image, 0, 0);
  LOG_IF(FATAL, !cursor) << "failed to load cursor.";
  return cursor;
}
}  // namespace prt

#endif  // PRT_GLFW