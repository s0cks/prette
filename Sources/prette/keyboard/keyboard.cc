#include "prette/keyboard/keyboard.h"

#include <sstream>
#include "prette/thread_local.h"

namespace prt::keyboard {
  static ThreadLocal<Keyboard> keyboard_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  Keyboard::Keyboard(engine::Engine* engine):
    Input(),
    KeyboardEventSource(),
    PreTickEventListener(engine),
    keys_(),
    state_() {
    state_.reset();
  }

  void Keyboard::ProcessKey(const int idx, const Key& k) { 
    const auto code = k.GetCode();
    const auto old_state = KeyState(state_.test(idx));
    const auto new_state = GetKeyState(k);
    if(old_state == new_state)
      return; // no changes
    state_.set(idx, (bool) new_state);
    if(new_state) {
      Publish<KeyPressedEvent>(this, k);
    } else {
      Publish<KeyReleasedEvent>(this, k);
    }
  }

  void Keyboard::ProcessKeys() {
    int idx = 0;
    for(const auto& key : keys_) {
      try {
        ProcessKey(idx++, key);
      } catch(const std::exception& e) {
        LOG(FATAL) << "error: " << e.what();
      }
    }
  }

  void Keyboard::PublishEvent(KeyboardEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

  auto Keyboard::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Keyboard(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  void Keyboard::OnPreTick(engine::PreTickEvent* event) {
    ProcessKeys();
  }

  auto Keyboard::New(Engine* engine) -> Keyboard* {
    PRT_ASSERT(engine);
    const auto kb = new Keyboard(engine);
    PRT_ASSERT(kb);
    kb->Publish<KeyboardCreatedEvent>(kb);
    return kb;
  }

  auto GetKeyboard() -> Keyboard* {
    return keyboard_.Get();
  }

  void InitKeyboard() {
    LOG_IF(FATAL, HasKeyboard()) << "cannot re-initialize Keyboard.";
    DLOG(INFO) << "initializing Keyboard....";
    keyboard_ = Keyboard::New(GetEngine());
  }
}