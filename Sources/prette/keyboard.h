#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <glfw/glfw3.h>

#include "prette/common.h"

namespace prt {
class Keyboard;
class KeyboardEvent {
 private:
  Keyboard* keyboard_;

 protected:
  explicit KeyboardEvent(Keyboard* keyboard) :
    keyboard_(keyboard) {
    ASSERT(keyboard_);
  }

 public:
  virtual ~KeyboardEvent() = default;
};

class Window;
class Keyboard {
 public:
  static void Init(Window* window);
};
}  // namespace prt

#endif  // KEYBOARD_H
