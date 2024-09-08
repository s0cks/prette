#ifndef PRT_KEYBOARD_MODE_H
#define PRT_KEYBOARD_MODE_H

#include <string>
#include <ostream>
#include <optional>
#include "prette/common.h"

namespace prt::keyboard {
#define FOR_EACH_KEYBOARD_MODE(V)   \
  V(Disabled)                       \
  V(Enabled)

  enum class Mode {
#define DEFINE_MODE(Name) k##Name,
    FOR_EACH_KEYBOARD_MODE(DEFINE_MODE)
#undef DEFINE_MODE
  };

  static inline auto
  operator<<(std::ostream& stream, const Mode& rhs) -> std::ostream& {
    switch(rhs) {
#define DEFINE_TO_STRING(Name)                      \
      case Mode::k##Name: return stream << #Name;
      FOR_EACH_KEYBOARD_MODE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown keyboard::Mode";
    }
  }

  static inline auto
  ParseMode(const std::string& rhs) -> std::optional<Mode> {
    if(rhs.empty())
      return std::nullopt;
#define DEFINE_PARSE(Name)                  \
    else if(EqualsIgnoreCase(rhs, #Name))   \
      return { Mode::k##Name };
    FOR_EACH_KEYBOARD_MODE(DEFINE_PARSE)
#undef DEFINE_PARSE
    LOG(ERROR) << "Unknown keyboard::Mode: " << rhs;
    return std::nullopt;
  }
}

#endif // PRT_KEYBOARD_MODE_H