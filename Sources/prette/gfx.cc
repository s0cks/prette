#include "prette/gfx.h"

#include "prette/prette.h"

namespace prt::gfx {
void Init() {
#ifdef PRETTE_ENABLE_LUA
  glfwSetErrorCallback(&prt::OnGlfwError);
  LOG_IF(FATAL, !glfwInit()) << "error initializing GLFW";
#endif  // PRETTE_ENABLE_LUA
}
}  // namespace prt::gfx