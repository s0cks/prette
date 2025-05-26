#ifndef PRT_MOUSE_H
#define PRT_MOUSE_H

#include <gflags/gflags.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/glm.h"
#include "prette/mouse/mouse_event.h"
#include "prette/mouse/mouse_settings.h"
#include "prette/rx.h"

namespace prt {
auto OnMouseEvent() -> MouseEventObservable;

static inline auto OnDragEvent() -> DragEventObservable {
  return OnMouseEvent().filter(DragEvent::Filter).map(DragEvent::Cast);
}

#define DEFINE_ON_EVENT(Name)                                                 \
  static inline auto On##Name##Event()->Name##EventObservable {               \
    return OnMouseEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                           \
  template <class... ArgN>                                                    \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {     \
    return On##Name##Event().subscribe(args...);                              \
  }
FOR_EACH_MOUSE_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

static inline auto OnMouseButton(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByButton(button)).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMousePressed() -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByPressed()).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMousePressed(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByPressed(button)).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMouseReleased() -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByReleased()).map(MouseButtonStateEvent::Cast);
}

static inline auto OnMouseReleased(const int button) -> MouseButtonStateEventObservable {
  return OnMouseEvent().filter(MouseButtonStateEvent::FilterByReleased(button)).map(MouseButtonStateEvent::Cast);
}

class Window;
class Mouse {
#ifdef PRT_GLFW
  static auto Unwrap(GLFWwindow* handle) -> Mouse*;
  static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void OnMouseMotion(GLFWwindow* window, double xPos, double yPos);
  static void OnMouseScroll(GLFWwindow* window, double xOff, double yOffset);
#endif  // PRT_GLFW

 private:
  MouseSettings settings_;
  Window* owner_;
  glm::vec2 previous_pos_{};

  bool dragging_ = false;
  DragState drag_state_{};

  explicit Mouse(Window* owner);

  void StartDragging(const int button, const glm::vec2 pos, const int mods);

  inline void StartDragging(const int button, const int mods) {
    return StartDragging(button, GetPos(), mods);
  }

  void StopDragging(const glm::vec2 pos);

  inline void StopDragging() {
    return StopDragging(GetPos());
  }

  static void PublishEvent(MouseEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

 public:
  ~Mouse();

  auto GetSettings() const -> const MouseSettings& {
    return settings_;
  }

  auto GetOwner() const -> Window* {
    return owner_;
  }

  auto IsDragging() const -> bool {
    return dragging_;
  }

  auto GetPreviousPos() const -> glm::vec2 const& {
    return previous_pos_;
  }

  auto GetPos() const -> glm::dvec2;
  auto GetWorldPos() const -> glm::vec2;
  auto IsPressed(const int btn) const -> bool;
  auto GetNormalizedDeviceCoords() const -> glm::vec2;

 private:
  static auto New(Window* owner) -> Mouse*;
#ifdef PRT_ENABLE_LUA
  static void InitLua(lua_State* L);
#endif  // PRT_ENABLE_LUA

 public:
  static auto IsInitialized() -> bool;
  static auto Init(Window* owner) -> Mouse*;
  static auto Get() -> Mouse*;
};

static inline auto GetMouseSettings() -> MouseSettings const& {
  ASSERT(Mouse::IsInitialized());
  return Mouse::Get()->GetSettings();
}
}  // namespace prt

#endif  // PRT_MOUSE_H
