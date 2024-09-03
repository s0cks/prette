
#ifndef PRT_MOUSE_FLAGS_H
#define PRT_MOUSE_FLAGS_H

#include <fmt/format.h>
#include "prette/flags.h"

namespace prt::mouse {
static constexpr const auto kDefaultMouseFlagValue = true;
DECLARE_bool(mouse);

static constexpr const auto kDefaultCursorsDir = "";
DECLARE_string(cursors_dir);

  static inline std::string
  GetCursorsDir() {
    if(!FLAGS_cursors_dir.empty())
      return FLAGS_cursors_dir;
    return fmt::format("{0:s}/cursors", FLAGS_resources);
  }

  static inline bool IsMouseEnabled() {
    return FLAGS_mouse == true;
  }

  static inline bool IsMouseDisabled() {
    return FLAGS_mouse == false;
  }
}

#endif //PRT_MOUSE_FLAGS_H