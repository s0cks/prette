#ifndef PRT_MOUSE_H
#define PRT_MOUSE_H

#include <utility>
#include <optional>
#include "prette/shape.h"
#include "prette/input.h"
#include "prette/mouse/cursor_mode.h"
#include "prette/mouse/mouse_flags.h"
#include "prette/mouse/mouse_events.h"
#include "prette/mouse/mouse_button.h"
#include "prette/engine/engine_event_listener.h"

namespace prt {
  namespace window {
    class Window;
  }

  namespace engine {
    class Engine;
  }

  namespace mouse {
    class Mouse : public Input,
                  public MouseEventSource,
                  protected engine::PreTickEventListener {
      friend class engine::Engine;
      DEFINE_NON_COPYABLE_TYPE(Mouse);
    private:
      MouseEventSubject events_;
      Point pos_;
      Point last_pos_;
      MouseButtonSet buttons_;
      MouseButtonBitset states_;

      void PublishEvent(MouseEvent* event) override;
      void OnPreTick(engine::PreTickEvent* event) override;
      virtual void SetCursorMode(const CursorMode mode);
      virtual void SetCursorPos(const Point& pos);
      virtual void GetCursorPos(Point& pos) const;
    public:
      explicit Mouse(engine::Engine* engine);
      ~Mouse() override = default;

      auto GetCursorMode() const -> CursorMode;
#define DEFINE_CURSOR_MODE_CHECK(Name)                                                          \
      auto IsCursor##Name() const -> bool { return GetCursorMode() == CursorMode::k##Name; }
      FOR_EACH_CURSOR_MODE(DEFINE_CURSOR_MODE_CHECK)
#undef DEFINE_CURSOR_MODE_CHECK

      auto GetPos() const -> Point {
        return pos_;
      }

      auto GetLastPos() const -> Point {
        return last_pos_;
      }

      auto GetButtonState(const MouseButton::Code code) const -> MouseButton::State;

      inline auto GetButtonState(const MouseButton& rhs) const -> MouseButton::State {
        return GetButtonState(rhs.GetCode());
      }

      inline auto IsButtonPressed(const MouseButton::Code code) const -> bool {
        return GetButtonState(code) == MouseButton::kPressed;
      }

      inline auto IsButtonPressed(const MouseButton& rhs) const -> bool {
        return GetButtonState(rhs) == MouseButton::kPressed;
      }

      inline auto IsButtonReleased(const MouseButton::Code code) const -> bool {
        return GetButtonState(code) == MouseButton::kReleased;
      }

      inline auto IsButtonReleased(const MouseButton& rhs) const -> bool {
        return GetButtonState(rhs) == MouseButton::kReleased;
      }

      inline void GetButtonStates(MouseButtonBitset& states) const {
        for(const auto btn : buttons_)
          states[btn.GetCode()] = IsButtonPressed(btn);
      }

      virtual auto GetAllButtons() const -> rx::observable<MouseButton> {
        return rx::observable<>::iterate(buttons_);
      }

      auto OnEvent() const -> MouseEventObservable override {
        return events_.get_observable();
      }

      auto ToString() const -> std::string override;
    public:
      static Mouse* New(Engine* engine);
    };

    void InitMouse();
    auto GetMouse() -> Mouse*;

    static inline auto
    HasMouse() -> bool {
      return GetMouse() != nullptr;
    }
  }

  using mouse::Mouse;
  using mouse::GetMouse;
}

#include "prette/mouse/mouse_logger.h"

#endif //PRT_MOUSE_H