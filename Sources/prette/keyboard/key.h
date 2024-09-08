#ifndef PRT_KEY_CODE_H
#define PRT_KEY_CODE_H

#include <set>
#include <bitset>
#include <utility>

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt::keyboard {
  using KeyCode = int32_t;

#ifdef PRT_GLFW
  static constexpr const int32_t kKeyUnknown = GLFW_KEY_UNKNOWN;
  static constexpr const int32_t kTotalNumberOfKeys = 120;
#else
#error "Unsupported Platform."
#endif //PRT_GLFW

  class Key {
    friend class KeySet;
    DEFINE_DEFAULT_COPYABLE_TYPE(Key);
  private:
    KeyCode code_;
  public:
    constexpr Key(const KeyCode code = kKeyUnknown):
      code_(code) {
    }
    ~Key() = default;

    constexpr auto GetCode() const -> KeyCode {
      return code_;
    }

    auto GetName() const -> std::string;

    constexpr auto IsUnknown() const -> bool {
      return GetCode() == kKeyUnknown;
    }

    inline constexpr auto IsCode(const KeyCode code) const -> bool {
      return GetCode() == code;
    }

    constexpr auto operator<(const Key& rhs) const -> bool {
      return GetCode() < rhs.GetCode();
    }

    constexpr auto operator>(const Key& rhs) const -> bool {
      return GetCode() > rhs.GetCode();
    }

    constexpr auto operator==(const Key& rhs) const -> bool {
      return GetCode() == rhs.GetCode();
    }

    constexpr auto operator==(const KeyCode rhs) const -> bool {
      return GetCode() == rhs;
    }

    constexpr auto operator!=(const Key& rhs) const -> bool {
      return GetCode() != rhs.GetCode();
    }

    constexpr auto operator!=(const KeyCode rhs) const -> bool {
      return GetCode() != rhs;
    }

    friend auto operator<<(std::ostream& stream, const Key& rhs) -> std::ostream& {
      stream << "Key(";
      stream << "name=" << rhs.GetName() << ", ";
      stream << "code=" << rhs.GetCode();
      stream << ")";
      return stream;
    }
  };

  using KeyStateSet = std::bitset<kTotalNumberOfKeys>;

  class KeyState {
    DEFINE_DEFAULT_COPYABLE_TYPE(KeyState);
  public:
    using RawType = int;
#ifdef PRT_GLFW

    static constexpr const RawType kPressed = GLFW_PRESS;
    static constexpr const RawType kReleased = GLFW_RELEASE;

#else
#error "Unsupported Platform."
#endif //PRT_GLFW
  private:
    RawType value_;
  public:
    constexpr KeyState(const RawType raw = kReleased):
      value_(raw) {
    }
    constexpr explicit KeyState(const bool value):
      value_(value ? kPressed : kReleased) {
    }
    explicit KeyState(const KeyStateSet& states, const KeyCode code):
      KeyState(states.test(code)) {
    }
    explicit KeyState(const KeyStateSet& states, const Key& k):
      KeyState(states, k.GetCode()) {
    }
    ~KeyState() = default;

    auto value() const -> RawType {
      return value_;
    }

    constexpr auto IsPressed() const -> bool {
      return value() == GLFW_PRESS;
    }

    constexpr auto IsReleased() const -> bool {
      return value() == GLFW_RELEASE;
    }

    constexpr auto operator==(const KeyState& rhs) const -> bool {
      return value() == rhs.value();
    }

    constexpr auto operator!=(const KeyState& rhs) const -> bool {
      return value() != rhs.value();
    }

    constexpr auto operator<(const KeyState& rhs) const -> bool {
      return value() < rhs.value();
    }

    constexpr auto operator>(const KeyState& rhs) const -> bool {
      return value() < rhs.value();
    }

    constexpr explicit operator const char*() const {
      return IsPressed() ? "Pressed" : "Released";
    }

    explicit operator std::string() const {
      return IsPressed() ? "Pressed" : "Released";
    }

    constexpr operator bool() const {
      return IsPressed();
    }

    friend auto operator<<(std::ostream& stream, const KeyState& rhs) -> std::ostream& {
      return stream << rhs;
    }
  public:
    static constexpr auto
    Pressed() -> KeyState {
      return kPressed;
    }

    static constexpr auto
    Released() -> KeyState {
      return kReleased;
    }
  };

  class KeySet {
    DEFINE_NON_COPYABLE_TYPE(KeySet);
  private:
    struct Compare {
      auto operator()(const Key& lhs, const Key& rhs) const -> bool {
        return lhs.GetCode() < rhs.GetCode();
      }
    };
    using SetType = std::set<Key>;
  private:
    SetType all_;

    inline void Insert(const Key& k) {
      const auto pos = all_.insert(k);
      LOG_IF(WARNING, !pos.second) << "failed to insert " << k << " into KeySet.";
    }

    inline void Insert(const KeyCode code) {
      return Insert(Key(code));
    }
  public:
    KeySet();
    ~KeySet() = default;

    auto Contains(const Key& k) const -> bool {
      const auto pos = all_.find(k);
      return pos != end();
    }

    auto Contains(const KeyCode code) const -> bool {
      return Contains(Key(code));
    }

    auto size() const -> size_t {
      return all_.size();
    }

    auto begin() const -> SetType::const_iterator {
      return all_.begin();
    }

    auto end() const -> SetType::const_iterator {
      return all_.end();
    }

    auto ToObservable() const -> rx::observable<Key> {
      return rx::observable<>::iterate(all_);
    }
  };
}

#endif //PRT_KEY_CODE_H