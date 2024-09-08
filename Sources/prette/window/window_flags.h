#ifndef PRT_WINDOW_FLAGS_H
#define PRT_WINDOW_FLAGS_H

#include "prette/flags.h"
#include "prette/dimension.h"

namespace prt::window {
static constexpr const auto kDefaultWindowSize = "512x512";
DECLARE_string(window_size); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  HasWindowSize() -> bool {
    return !FLAGS_window_size.empty();
  }

  static inline auto
  GetWindowSize() -> Dimension {
    return Dimension(FLAGS_window_size);
  }
}

#endif //PRT_WINDOW_FLAGS_H