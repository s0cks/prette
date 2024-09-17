#ifndef PRT_CURSOR_MODE_H
#define PRT_CURSOR_MODE_H

#include <ostream>
#include <optional>
#include "prette/common.h"

namespace prt::mouse {
#define FOR_EACH_CURSOR_MODE(V) \
  V(Normal)                     \
  V(Hidden)                     \
  V(Disabled)                   \
  V(Captured)                   \
  V(Unavailable)

  enum CursorMode : uint8_t {
#define DEFINE_MODE(Name) k##Name,
    FOR_EACH_CURSOR_MODE(DEFINE_MODE)
#undef DEFINE_MODE
    kDefaultCursorMode = kNormal,
  };

  static inline auto
  GetCursorModeName(const CursorMode& rhs) -> const char* {
    switch(rhs) {
#define TO_STRING(Name) \
      case k##Name: return #Name;
      FOR_EACH_CURSOR_MODE(TO_STRING)
#undef TO_STRING
      default: return "Unknown";
    }
  }

  static inline auto
  operator<<(std::ostream& stream, const CursorMode& rhs) -> std::ostream& {
    return stream << GetCursorModeName(rhs);
  }

  static inline auto
  ParseCursorMode(const std::string& value) -> std::optional<CursorMode> {
    if(EqualsIgnoreCase(value, "default"))
      return { kDefaultCursorMode };
#define PARSE_CURSOR_MODE(Name)                 \
    else if(EqualsIgnoreCase(value, #Name))     \
      return { k##Name };
    FOR_EACH_CURSOR_MODE(PARSE_CURSOR_MODE)
#undef DEFINE_PARSE_INPUT_MODE
    DLOG(ERROR) << "failed to parse CursorMode from: " << value;
    return std::nullopt;
  }
}

#endif //PRT_CURSOR_MODE_H