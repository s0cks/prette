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

      void ProcessKeys(const rx::subscriber<KeyboardEvent*>& s);
      void ProcessKey(const rx::subscriber<KeyboardEvent*>& s, int index, const Key& key);
      void OnPreTick(engine::PreTickEvent* event) override;

      inline void SetState(const KeyStateSet& state) {
        state_ = state;
      }

      static auto GetKeyState(const Key& key) -> KeyState;
    public:
      explicit Keyboard(engine::Engine* engine);
      ~Keyboard() override = default;

      auto GetKeys() const -> const KeySet& {
        return keys_;
      }

      auto GetState() const -> const KeyStateSet& {
        return state_;
      }

      auto ToString() const -> std::string override;
    };

    auto GetKeyboard() -> Keyboard*;
    void InitKeyboard();

    static inline auto
    HasKeyboard() -> bool {
      return GetKeyboard() != nullptr;
    }
  }
  using keyboard::Keyboard;
  using keyboard::GetKeyboard;
}

#endif //PRT_KEYBOARD_H