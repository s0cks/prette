
#ifndef PRT_MOUSE_FLAGS_H
#define PRT_MOUSE_FLAGS_H

#include <fmt/format.h>
#include <gflags/gflags.h>

#include "prette/flags.h"
#include "prette/mouse/mouse_mode.h"

namespace prt::mouse {
DECLARE_string(mouse_mode); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static constexpr const auto kDefaultCursorsDir = "";
DECLARE_string(cursors_dir); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  GetMouseMode() -> std::optional<Mode> {
    return ParseMode(FLAGS_mouse_mode);
  }

  static inline auto
  GetCursorsDir() -> std::string {
    if(!FLAGS_cursors_dir.empty())
      return FLAGS_cursors_dir;
    return fmt::format("{0:s}/cursors", FLAGS_resources);
  }
}

#endif //PRT_MOUSE_FLAGS_H