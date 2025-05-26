#include "prette/gfx.h"

#include "prette/prette.h"

namespace prt::gfx {
void Init() {
#ifdef PRT_ENABLE_LUA
  glfwSetErrorCallback(&prt::OnGlfwError);
  LOG_IF(FATAL, !glfwInit()) << "error initializing GLFW";
#endif  // PRT_ENABLE_LUA
}
}  // namespace prt::gfx