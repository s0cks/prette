#include "prette/platform.h"
#ifdef PRT_GLFW

#include "prette/mouse.h"

namespace prt {
void Mouse::OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
  ASSERT(window);
}
}  // namespace prt

#endif  // PRT_GLFW