#include "prette/color.h"
#include "prette/gfx.h"

namespace prt::gfx {
  static constexpr const auto kComponentMax = 255.0f;

  void SetClearColor(const Color& c) {
    const auto r = static_cast<float>(c[0]) / kComponentMax;
    const auto g = static_cast<float>(c[1]) / kComponentMax;
    const auto b = static_cast<float>(c[2]) / kComponentMax;
    const auto a = static_cast<float>(c[3]) / kComponentMax;
    glClearColor(r, g, b, a);
    CHECK_GL;
  }
}