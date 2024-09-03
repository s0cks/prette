#include "prette/keyboard/keyboard.h"

#include "prette/uv/utils.h"
#include "prette/thread_local.h"
#include "prette/window/window.h"

namespace prt::keyboard {
  static KeyboardEventSubject events_;
  static ThreadLocal<Keyboard> keyboard_;

  template<typename E>
  static inline void
  Publish(Keyboard* keyboard) {
    E event(keyboard);
    return events_.get_subscriber().on_next(&event);
  }

  Keyboard* GetKeyboard() {
    return keyboard_.Get();
  }

  static inline void
  SetKeyboard(Keyboard* kb) {
    PRT_ASSERT(kb);
    keyboard_.Set(kb);
  }

  static inline Keyboard*
  CreateKeyboard(engine::Engine* engine) {
    PRT_ASSERT(engine);
    const auto keyboard = new Keyboard(engine);
    Publish<KeyboardCreatedEvent>(keyboard);
    return keyboard;
  }

  void InitKeyboard() {
    DLOG(INFO) << "initializing Keyboard....";
    const auto engine = engine::GetEngine();
    PRT_ASSERT(engine);
    SetKeyboard(CreateKeyboard(engine));
  }

  void Keyboard::ProcessKey(const rx::subscriber<KeyboardEvent*>& s, const int idx, const Key& k) { 
    const auto code = k.GetCode();
    const auto old_state = KeyState(state_.test(idx));
    const auto new_state = GetKeyState(k);
    if(old_state == new_state)
      return; // no changes
    state_.set(idx, (bool) new_state);
    const auto event = new_state
      ? (new KeyPressedEvent(this, k))->AsKeyEvent()
      : (new KeyReleasedEvent(this, k))->AsKeyEvent();
    s.on_next(event);
  }

  void Keyboard::ProcessKeys(const rx::subscriber<KeyboardEvent*>& s) {
    int idx = 0;
    for(const auto& key : keys_) {
      try {
        ProcessKey(s, idx++, key);
      } catch(const std::exception& e) {
        LOG(FATAL) << "error: " << e.what();
      }
    }
  }

  Keyboard::Keyboard(engine::Engine* engine):
    Input(),
    KeyboardEventPublisher(keyboard::events_),
    PreTickEventListener(engine),
    keys_(),
    state_() {
    state_.reset();
  }

  std::string Keyboard::ToString() const {
    std::stringstream ss;
    ss << "Keyboard(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  rx::observable<KeyboardEvent*> OnKeyboardEvent() {
    return events_.get_observable();
  }

  void Keyboard::OnPreTick(engine::PreTickEvent* event) {
    const auto subscriber = GetSubscriber();
    ProcessKeys(subscriber);
  }
}