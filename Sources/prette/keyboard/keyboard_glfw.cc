#include "prette/keyboard/keyboard.h"
#ifdef PRT_GLFW

#include "prette/keyboard/key.h"
#include "prette/window/window.h"

namespace prt::keyboard {
  auto Keyboard::GetKeyState(const Key& key) -> KeyState {
    const auto window =  GetAppWindow();
    PRT_ASSERT(window);
    const auto handle = window->GetHandle();
    PRT_ASSERT(handle);
    return { glfwGetKey(handle, key.GetCode()) };
  }
}

#endif //PRT_GLFW