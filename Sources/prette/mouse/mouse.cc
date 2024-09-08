#include "prette/mouse/mouse.h"

#include <units.h>
#include "prette/thread_local.h"
#include "prette/window/window.h"
#include "prette/engine/engine.h"

namespace prt::mouse {
  static ThreadLocal<Mouse> mouse_;

  Mouse::Mouse(engine::Engine* engine):
    Input(),
    MouseEventSource(),
    engine::PreTickEventListener(engine),
    events_(),
    pos_(0),
    last_pos_(0),
    buttons_(),
    states_() {
    for(auto idx = 0; idx < kNumberOfMouseButtons; idx++)
      buttons_.insert(MouseButton(static_cast<MouseButton::Code>(idx)));
  }

  static inline auto
  SetMouse(Mouse* mouse) -> Mouse* {
    PRT_ASSERT(mouse);
    mouse_.Set(mouse);
    return mouse;
  }

  auto GetMouse() -> Mouse* {
    PRT_ASSERT(mouse_);
    return mouse_.Get();
  }

  auto Mouse::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Mouse(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  void InitMouse() {
    if(IsMouseDisabled()) {
      LOG(INFO) << "Mouse is disabled.";
      return;
    }

    DLOG(INFO) << "initializing Mouse....";
    const auto engine = engine::GetEngine();
    SetMouse(new Mouse(engine));
  }

  static inline auto
  HasMotion(const glm::vec2& delta) -> bool {
    return delta[0] != 0.0f && delta[1] != 0.0f;
  }

  void Mouse::PublishEvent(MouseEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  void Mouse::OnPreTick(engine::PreTickEvent* event) {
    if(!HasMouse())
      return;
    // update mouse pos_ & last_pos_
    GetCursorPos(pos_);
    // send mouse move event if has motion.
    const auto delta = (last_pos_ - pos_);
    if(HasMotion(delta)) {
      MouseEventSource::Publish<MouseMoveEvent>(this, pos_, last_pos_);
      std::swap(pos_, last_pos_);
    }

    // update mouse buttons
    for(const auto& btn : buttons_) {
      const auto idx = btn.GetCode();
      const auto old_state = states_[idx];
      const auto cur_state = IsButtonPressed(btn);
      if(old_state == cur_state)
        continue;

      if(cur_state == GLFW_PRESS) {
        MouseEventSource::Publish<MouseButtonPressedEvent>(this, pos_, btn);
        states_[idx] = GLFW_PRESS;
      } else if(cur_state == GLFW_RELEASE) {
        MouseEventSource::Publish<MouseButtonReleasedEvent>(this, pos_, btn);
        states_[idx] = GLFW_RELEASE;
      }
    }
  }
}