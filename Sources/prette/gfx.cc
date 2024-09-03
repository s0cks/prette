#include "prette/gfx.h"
#include "prette/prette.h"

namespace prt::gfx {
  void Init() {
    glfwSetErrorCallback(&prt::OnGlfwError);
    LOG_IF(FATAL, !glfwInit()) << "error initializing GLFW";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, PRT_GLFW_CTX_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, PRT_GLFW_CTX_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  }

  void Clear(const GLbitfield mask) {
    glClear(mask);
    CHECK_GL(FATAL);
  }
}