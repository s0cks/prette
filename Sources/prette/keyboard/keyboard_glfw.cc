#include "prette/keyboard/keyboard.h"
#ifdef PRT_GLFW

#include "prette/keyboard/key.h"
#include "prette/window/window.h"

namespace prt::keyboard {
  KeyState Keyboard::GetKeyState(const Key& key) {
    const auto window =  GetAppWindow();
    PRT_ASSERT(window);
    const auto handle = window->GetHandle();
    PRT_ASSERT(handle);
    return KeyState(glfwGetKey(handle, key.GetCode()));
  }
}

#endif //PRT_GLFW