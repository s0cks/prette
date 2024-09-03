#ifndef PRT_MOUSE_H
#define PRT_MOUSE_H

#include <utility>
#include <optional>

#include "prette/rx.h"
#include "prette/gfx.h"
#include "prette/shape.h"
#include "prette/input.h"
#include "prette/platform.h"
#include "prette/entity/entity.h"

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
    protected:
      MouseEventSubject events_;
      Point pos_;
      Point last_pos_;
      MouseButtonSet buttons_;
      MouseButtonBitset states_;

      void Publish(MouseEvent* event) override;
      void OnPreTick(engine::PreTickEvent* event) override;
      virtual void SetCursorMode(const CursorMode mode);
      virtual void SetCursorPos(const Point& pos);
      virtual void GetCursorPos(Point& pos) const;
    public:
      explicit Mouse(engine::Engine* engine);
      virtual ~Mouse();

      CursorMode GetCursorMode() const;
#define DEFINE_CURSOR_MODE_CHECK(Name)                                                  \
      bool IsCursor##Name() const { return GetCursorMode() == CursorMode::k##Name; }
      FOR_EACH_CURSOR_MODE(DEFINE_CURSOR_MODE_CHECK)
#undef DEFINE_CURSOR_MODE_CHECK

      Point GetPos() const {
        return pos_;
      }

      Point GetLastPos() const {
        return last_pos_;
      }

      MouseButton::State GetButtonState(const MouseButton::Code code) const;
      
      inline MouseButton::State GetButtonState(const MouseButton& rhs) const {
        return GetButtonState(rhs.GetCode());
      }

      inline bool IsButtonPressed(const MouseButton::Code code) const {
        return GetButtonState(code) == MouseButton::kPressed;
      }

      inline bool IsButtonPressed(const MouseButton& rhs) const {
        return GetButtonState(rhs) == MouseButton::kPressed;
      }

      inline bool IsButtonReleased(const MouseButton::Code code) const {
        return GetButtonState(code) == MouseButton::kReleased;
      }

      inline bool IsButtonReleased(const MouseButton& rhs) const {
        return GetButtonState(rhs) == MouseButton::kReleased;
      }

      inline void GetButtonStates(MouseButtonBitset& states) const {
        for(const auto btn : buttons_)
          states[btn.GetCode()] = IsButtonPressed(btn);
      }
      
      virtual rx::observable<MouseButton> GetAllButtons() const {
        return rx::observable<>::iterate(buttons_);
      }
      
      MouseEventObservable OnEvent() const override {
        return events_.get_observable();
      }

      std::string ToString() const override;
    };
    
    void InitMouse();
    Mouse* GetMouse();

    static inline bool
    HasMouse() {
      return IsMouseEnabled() && GetMouse() != nullptr;
    }

    static inline MouseEventObservable
    OnEvent() {
      const auto mouse = GetMouse();
      PRT_ASSERT(mouse);
      return mouse->OnEvent();
    }

#define DEFINE_ON_MOUSE_EVENT(Name)               \
    static inline Name##EventObservable           \
    On##Name##Event() {                           \
      return mouse::OnEvent()                     \
        .filter(Name##Event::Filter)              \
        .map(Name##Event::Cast);                  \
    }
    FOR_EACH_MOUSE_EVENT(DEFINE_ON_MOUSE_EVENT)
#undef DEFINE_ON_MOUSE_EVENT
  }
  
  using mouse::Mouse;
  using mouse::GetMouse;
//TODO: remove
#define DEFINE_USE_ON_MOUSE_EVENT(Name)             \
  using mouse::On##Name##Event;
  FOR_EACH_MOUSE_EVENT(DEFINE_USE_ON_MOUSE_EVENT)
#undef DEFINE_USE_ON_MOUSE_EVENT
//-------------------------------------------------------
}

#include "prette/mouse/mouse_logger.h"

#endif //PRT_MOUSE_H