#include "prette/input/input_system.h"

#include "prette/common.h"
#include "prette/mouse/mouse_system.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/thread_local.h"

#ifdef PRT_GLFW
#include <GLFW/glfw3.h>
#endif  // PRT_GLFW

namespace prt {
static ThreadLocal<InputSystem> system_{};

void InputSystem::Init() {
  ASSERT(!IsInitialized());
  DVLOG(1) << "initializing InputSystem.....";
  system_ = new InputSystem();
  MouseSystem::InitSystem();
}

auto InputSystem::IsInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto InputSystem::Get() -> InputSystem* {
  ASSERT(IsInitialized());
  return system_;
}

InputSystem::InputSystem() {
  on_pre_frame_ = OnPreFrame([this](PreFrameEvent* event) {
    PreFrame();
  });
}

void InputSystem::PreFrame() {
#ifdef PRT_GLFW
  glfwPollEvents();
#else
#error "Invalid input system"
#endif  // PRT_GLFW
}
}  // namespace prt