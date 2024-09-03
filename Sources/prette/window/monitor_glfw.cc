#include "prette/window/monitor.h"
#ifdef PRT_GLFW

namespace prt::window {
  const GLFWvidmode* Monitor::GetVideoMode() const {
    return glfwGetVideoMode(GetHandle());
  }

  Resolution Monitor::GetResolution() const {
    const auto mode = GetVideoMode();
    return Resolution(mode->width, mode->height);
  }

  int Monitor::GetRedBits() const {
    const auto mode = GetVideoMode();
    return mode->redBits;
  }

  int Monitor::GetBlueBits() const {
    const auto mode = GetVideoMode();
    return mode->blueBits;
  }

  int Monitor::GetGreenBits() const {
    const auto mode = GetVideoMode();
    return mode->greenBits;
  }

  int Monitor::GetRefreshRateInHertz() const {
    const auto mode = GetVideoMode();
    return mode->refreshRate;
  }

  std::string Monitor::GetName() const {
    const auto name = glfwGetMonitorName(GetHandle());
    return std::string(name);
  }
}

#endif //PRT_GLFW