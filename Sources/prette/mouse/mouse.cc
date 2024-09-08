#include "prette/mouse/mouse.h"

#include <units.h>
#include "prette/thread_local.h"
#include "prette/engine/engine.h"

namespace prt::mouse {
  static ThreadLocal<Mouse> mouse_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

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

  auto Mouse::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Mouse(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
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
#ifdef PRT_DEBUG
    const auto start_ns = uv::Now();
#endif //PRT_DEBUG
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
#ifdef PRT_DEBUG
    using units::time::nanosecond_t;
    const auto stop_ns = uv::Now();
    const auto total_ns = (stop_ns - start_ns);
    DLOG_EVERY_T(INFO, 30) << __FUNCTION__ << " finished in " << nanosecond_t(total_ns); // NOLINT(cppcoreguidelines-narrowing-conversions)
#endif //PRT_DEBUG
  }

  auto Mouse::New(Engine* engine) -> Mouse* {
    const auto mouse = new Mouse(engine);
    PRT_ASSERT(mouse);
    mouse->Publish<MouseCreatedEvent>(mouse);
    return mouse;
  }

  auto GetMouse() -> Mouse* {
    return mouse_.Get();
  }

  void InitMouse() {
#ifdef PRT_DEBUG
    const auto start_ns = uv::Now();
#endif //PRT_DEBUG
    const auto mode = GetMouseMode();
    if(mode == Mode::kDisabled) {
      DLOG(WARNING) << "Mouse is disabled.";
      return;
    }
    PRT_ASSERT(mode == Mode::kEnabled);
    LOG_IF(FATAL, HasMouse()) << "cannot re-initialize Mouse.";
    DLOG(INFO) << "initializing Mouse....";
    mouse_ = Mouse::New(GetEngine());
#ifdef PRT_DEBUG
    using units::time::nanosecond_t;
    const auto stop_ns = uv::Now();
    const auto total_ns = (stop_ns - start_ns);
    DLOG(INFO) << "Mouse initialized in " << nanosecond_t(total_ns); // NOLINT(cppcoreguidelines-narrowing-conversions)
#endif //PRT_DEBUG
  }
}