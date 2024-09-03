#ifndef PRT_KEY_CODE_H
#define PRT_KEY_CODE_H

#include <set>
#include <bitset>
#include <utility>

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt::keyboard {
  typedef int32_t KeyCode;

#ifdef PRT_GLFW
  static constexpr const int32_t kKeyUnknown = GLFW_KEY_UNKNOWN;
  static constexpr const int32_t kTotalNumberOfKeys = 120;
#else
#error "Unsupported Platform."
#endif //PRT_GLFW

  class Key {
    friend class KeySet;
  protected:
    KeyCode code_;
  public:
    constexpr Key(const KeyCode code = kKeyUnknown):
      code_(code) {
    }
    constexpr Key(const Key& rhs) = default;
    ~Key() = default;

    constexpr KeyCode GetCode() const {
      return code_;
    }

    std::string GetName() const;

    constexpr bool IsUnknown() const {
      return GetCode() == kKeyUnknown;
    }

    inline constexpr bool IsCode(const KeyCode code) const {
      return GetCode() == code;
    }

    constexpr Key& operator=(const Key& rhs) = default;
    
    constexpr bool operator<(const Key& rhs) const {
      return GetCode() < rhs.GetCode();
    }

    constexpr bool operator>(const Key& rhs) const {
      return GetCode() > rhs.GetCode();
    }

    constexpr bool operator==(const Key& rhs) const {
      return GetCode() == rhs.GetCode();
    }

    constexpr bool operator==(const KeyCode rhs) const {
      return GetCode() == rhs;
    }

    constexpr bool operator!=(const Key& rhs) const {
      return GetCode() != rhs.GetCode();
    }

    constexpr bool operator!=(const KeyCode rhs) const {
      return GetCode() != rhs;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Key& rhs) {
      stream << "Key(";
      stream << "name=" << rhs.GetName() << ", ";
      stream << "code=" << rhs.GetCode();
      stream << ")";
      return stream;
    }
  };

  typedef std::bitset<kTotalNumberOfKeys> KeyStateSet;

  class KeyState {
  public:
    typedef int RawType;
#ifdef PRT_GLFW

    static constexpr const RawType kPressed = GLFW_PRESS;
    static constexpr const RawType kReleased = GLFW_RELEASE;

#else
#error "Unsupported Platform."
#endif //PRT_GLFW
  protected:
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
    constexpr KeyState(const KeyState& rhs) = default;
    ~KeyState() = default;

    RawType value() const {
      return value_;
    }

    constexpr bool IsPressed() const {
      return value() == GLFW_PRESS;
    }

    constexpr bool IsReleased() const {
      return value() == GLFW_RELEASE;
    }

    KeyState& operator=(const KeyState& rhs) = default;

    constexpr bool operator==(const KeyState& rhs) const {
      return value() == rhs.value();
    }
    
    constexpr bool operator!=(const KeyState& rhs) const {
      return value() != rhs.value();
    }

    constexpr bool operator<(const KeyState& rhs) const {
      return value() < rhs.value();
    }

    constexpr bool operator>(const KeyState& rhs) const {
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

    friend std::ostream& operator<<(std::ostream& stream, const KeyState& rhs) {
      return stream << rhs;
    }
  public:
    static constexpr KeyState
    Pressed() {
      return kPressed;
    }

    static constexpr KeyState
    Released() {
      return kReleased;
    }
  };

  class KeySet {
    DEFINE_NON_COPYABLE_TYPE(KeySet);
  private:
    struct Compare {
      bool operator()(const Key& lhs, const Key& rhs) const {
        return lhs.GetCode() < rhs.GetCode();
      }
    };
    typedef std::set<Key> SetType;
  protected:
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

    bool Contains(const Key& k) const {
      const auto pos = all_.find(k);
      return pos != end();
    }

    bool Contains(const KeyCode code) const {
      return Contains(Key(code));
    }

    size_t size() const {
      return all_.size();
    }

    SetType::const_iterator begin() const {
      return all_.begin();
    }

    SetType::const_iterator end() const {
      return all_.end();
    }

    rx::observable<Key> ToObservable() const {
      return rx::observable<>::iterate(all_);
    }
  };
}

#endif //PRT_KEY_CODE_H