#ifndef PRT_KEYBOARD_H
#define PRT_KEYBOARD_H

#include "prette/input.h"
#include "prette/keyboard/keyboard_event.h"
#include "prette/engine/engine_event_listener.h"

namespace prt {
  namespace engine {
    class Engine;
  }

  namespace keyboard {
    class Keyboard : public Input,
                     public KeyboardEventSource,
                     public engine::PreTickEventListener {
      friend class engine::Engine;
      DEFINE_NON_COPYABLE_TYPE(Keyboard);
    private:
      KeySet keys_;
      KeyStateSet state_;
      KeyboardEventSubject events_;

      void ProcessKeys();
      void ProcessKey(int index, const Key& key);
      void OnPreTick(engine::PreTickEvent* event) override;

      inline void SetState(const KeyStateSet& state) {
        state_ = state;
      }

      static auto GetKeyState(const Key& key) -> KeyState;
      void PublishEvent(KeyboardEvent* event) override;
    public:
      explicit Keyboard(engine::Engine* engine);
      ~Keyboard() override = default;

      auto GetKeys() const -> const KeySet& {
        return keys_;
      }

      auto GetState() const -> const KeyStateSet& {
        return state_;
      }

      auto OnEvent() const -> KeyboardEventObservable override {
        return events_.get_observable();
      }

      auto ToString() const -> std::string override;
    public:
      static auto New(Engine* engine) -> Keyboard*;
    };

    void InitKeyboard();
    auto GetKeyboard() -> Keyboard*;

    static inline auto
    HasKeyboard() -> bool {
      return GetKeyboard() != nullptr;
    }
  }
  using keyboard::Keyboard;
  using keyboard::GetKeyboard;
}

#endif //PRT_KEYBOARD_H