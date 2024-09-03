#ifndef PRT_KEYBOARD_H
#define PRT_KEYBOARD_H

#include "prette/rx.h"
#include "prette/gfx.h"
#include "prette/uuid.h"
#include "prette/input.h"
#include "prette/keyboard/keyboard_event.h"
#include "prette/engine/engine_event_listener.h"

namespace prt {
  namespace engine {
    class Engine;
  }

  namespace keyboard {
    rx::observable<KeyboardEvent*> OnKeyboardEvent();
#define DEFINE_ON_KEYBOARD_EVENT(Name)                  \
    static inline rx::observable<Name##Event*>          \
    On##Name##Event() {                                 \
      return OnKeyboardEvent()                          \
        .filter(Name##Event::Filter)                    \
        .map(Name##Event::Cast);                        \
    }
    FOR_EACH_KEYBOARD_EVENT(DEFINE_ON_KEYBOARD_EVENT)
#undef DEFINE_ON_KEYBOARD_EVENT

    class Keyboard : public Input,
                     public KeyboardEventPublisher,
                     public engine::PreTickEventListener {
      friend class engine::Engine;
    protected:
      KeySet keys_;
      KeyStateSet state_;

      void ProcessKeys(const rx::subscriber<KeyboardEvent*>& s);
      void ProcessKey(const rx::subscriber<KeyboardEvent*>& s, int index, const Key& key);
      void OnPreTick(engine::PreTickEvent* event) override;

      inline void SetState(const KeyStateSet& state) {
        state_ = state;
      }

      static KeyState GetKeyState(const Key& key);
    public:
      explicit Keyboard(engine::Engine* engine);
      ~Keyboard() override = default;

      const KeySet& GetKeys() const {
        return keys_;
      }

      const KeyStateSet& GetState() const {
        return state_;
      }

      std::string ToString() const override;
    };

    Keyboard* GetKeyboard();
    void InitKeyboard();

    static inline bool
    HasKeyboard() {
      return GetKeyboard() != nullptr;
    }
  }
  using keyboard::Keyboard;
  using keyboard::GetKeyboard;
}

#endif //PRT_KEYBOARD_H