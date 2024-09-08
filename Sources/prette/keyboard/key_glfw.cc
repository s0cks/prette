#include "prette/keyboard/key.h"
#ifdef PRT_GLFW

namespace prt::keyboard {
  auto Key::GetName() const -> std::string {
    const auto name = glfwGetKeyName(GetCode(), 0);
    return name ? std::string(name) : std::string{};
  }

  KeySet::KeySet():
    all_() {
    Insert(GLFW_KEY_SPACE);
    Insert(GLFW_KEY_APOSTROPHE);
    Insert(GLFW_KEY_COMMA);
    Insert(GLFW_KEY_MINUS);
    Insert(GLFW_KEY_PERIOD);
    Insert(GLFW_KEY_SLASH);
    Insert(GLFW_KEY_0);
    Insert(GLFW_KEY_1);
    Insert(GLFW_KEY_2);
    Insert(GLFW_KEY_3);
    Insert(GLFW_KEY_4);
    Insert(GLFW_KEY_5);
    Insert(GLFW_KEY_6);
    Insert(GLFW_KEY_7);
    Insert(GLFW_KEY_8);
    Insert(GLFW_KEY_9);
    Insert(GLFW_KEY_SEMICOLON);
    Insert(GLFW_KEY_EQUAL);
    Insert(GLFW_KEY_A);
    Insert(GLFW_KEY_B);
    Insert(GLFW_KEY_C);
    Insert(GLFW_KEY_D);
    Insert(GLFW_KEY_E);
    Insert(GLFW_KEY_F);
    Insert(GLFW_KEY_G);
    Insert(GLFW_KEY_H);
    Insert(GLFW_KEY_I);
    Insert(GLFW_KEY_J);
    Insert(GLFW_KEY_K);
    Insert(GLFW_KEY_L);
    Insert(GLFW_KEY_M);
    Insert(GLFW_KEY_N);
    Insert(GLFW_KEY_O);
    Insert(GLFW_KEY_P);
    Insert(GLFW_KEY_Q);
    Insert(GLFW_KEY_R);
    Insert(GLFW_KEY_S);
    Insert(GLFW_KEY_T);
    Insert(GLFW_KEY_U);
    Insert(GLFW_KEY_V);
    Insert(GLFW_KEY_W);
    Insert(GLFW_KEY_X);
    Insert(GLFW_KEY_Y);
    Insert(GLFW_KEY_Z);
    Insert(GLFW_KEY_LEFT_BRACKET);
    Insert(GLFW_KEY_BACKSLASH);
    Insert(GLFW_KEY_RIGHT_BRACKET);
    Insert(GLFW_KEY_GRAVE_ACCENT);
    // Insert(GLFW_KEY_WORLD_1);
    // Insert(GLFW_KEY_WORLD_2);
    Insert(GLFW_KEY_ESCAPE);
    Insert(GLFW_KEY_ENTER);
    Insert(GLFW_KEY_TAB);
    Insert(GLFW_KEY_BACKSPACE);
    Insert(GLFW_KEY_INSERT);
    Insert(GLFW_KEY_DELETE);
    Insert(GLFW_KEY_RIGHT);
    Insert(GLFW_KEY_LEFT);
    Insert(GLFW_KEY_DOWN);
    Insert(GLFW_KEY_UP);
    Insert(GLFW_KEY_PAGE_UP);
    Insert(GLFW_KEY_PAGE_DOWN);
    Insert(GLFW_KEY_HOME);
    Insert(GLFW_KEY_END);
    Insert(GLFW_KEY_CAPS_LOCK);
    Insert(GLFW_KEY_SCROLL_LOCK);
    Insert(GLFW_KEY_NUM_LOCK);
    Insert(GLFW_KEY_PRINT_SCREEN);
    Insert(GLFW_KEY_PAUSE);
    Insert(GLFW_KEY_F1);
    Insert(GLFW_KEY_F2);
    Insert(GLFW_KEY_F3);
    Insert(GLFW_KEY_F4);
    Insert(GLFW_KEY_F5);
    Insert(GLFW_KEY_F6);
    Insert(GLFW_KEY_F7);
    Insert(GLFW_KEY_F8);
    Insert(GLFW_KEY_F9);
    Insert(GLFW_KEY_F10);
    Insert(GLFW_KEY_F11);
    Insert(GLFW_KEY_F12);
    Insert(GLFW_KEY_F13);
    Insert(GLFW_KEY_F14);
    Insert(GLFW_KEY_F15);
    Insert(GLFW_KEY_F16);
    Insert(GLFW_KEY_F17);
    Insert(GLFW_KEY_F18);
    Insert(GLFW_KEY_F19);
    Insert(GLFW_KEY_F20);
    Insert(GLFW_KEY_F21);
    Insert(GLFW_KEY_F22);
    Insert(GLFW_KEY_F23);
    Insert(GLFW_KEY_F24);
    Insert(GLFW_KEY_F25);
    Insert(GLFW_KEY_KP_0);
    Insert(GLFW_KEY_KP_1);
    Insert(GLFW_KEY_KP_2);
    Insert(GLFW_KEY_KP_3);
    Insert(GLFW_KEY_KP_4);
    Insert(GLFW_KEY_KP_5);
    Insert(GLFW_KEY_KP_6);
    Insert(GLFW_KEY_KP_7);
    Insert(GLFW_KEY_KP_8);
    Insert(GLFW_KEY_KP_9);
    Insert(GLFW_KEY_KP_DECIMAL);
    Insert(GLFW_KEY_KP_DIVIDE);
    Insert(GLFW_KEY_KP_MULTIPLY);
    Insert(GLFW_KEY_KP_SUBTRACT);
    Insert(GLFW_KEY_KP_ADD);
    Insert(GLFW_KEY_KP_ENTER);
    Insert(GLFW_KEY_KP_EQUAL);
    Insert(GLFW_KEY_LEFT_SHIFT);
    Insert(GLFW_KEY_LEFT_CONTROL);
    Insert(GLFW_KEY_LEFT_ALT);
    Insert(GLFW_KEY_LEFT_SUPER);
    Insert(GLFW_KEY_RIGHT_SHIFT);
    Insert(GLFW_KEY_RIGHT_CONTROL);
    Insert(GLFW_KEY_RIGHT_ALT);
    Insert(GLFW_KEY_RIGHT_SUPER);
    Insert(GLFW_KEY_MENU);
  }
}

#endif //PRT_GLFW