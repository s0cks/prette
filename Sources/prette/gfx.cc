#include "prette/gfx.h"
#include "prette/prette.h"

namespace prt::gfx {
  void Init() {
    glfwSetErrorCallback(&prt::OnGlfwError);
    LOG_IF(FATAL, !glfwInit()) << "error initializing GLFW";
  }
}