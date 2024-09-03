#ifndef PRT_RENDER_FLAGS_H
#define PRT_RENDER_FLAGS_H

#include "prette/flags.h"
#include "prette/resolution.h"

namespace prt::render {
  static constexpr const auto kDefaultResolutionString = "";
  DECLARE_string(resolution);

  static const auto kSDResolution = Resolution(854, 480);
  static const auto kHDResolution = Resolution(1280, 720);
  static const auto kFullHDResolution = Resolution(1920, 1080);
  static const auto k2kResolution = Resolution(2560, 1440);
  static const auto k4kResolution = Resolution(3840, 2160);
  static const auto k5kResolution = Resolution(5120, 2880);
  static const auto k8kResolution = Resolution(7680, 4320);

  static const auto kDefaultTargetResolution = kHDResolution;

  //TODO: deprecated
  static inline Resolution
  GetDefaultResolution() {
    return Resolution(FLAGS_resolution);
  }

  static inline bool
  HasTargetResolution() {
    return !FLAGS_resolution.empty();
  }

  static inline Resolution
  GetTargetResolution() {
    if(!HasTargetResolution())
      return kDefaultTargetResolution;
    return Resolution(FLAGS_resolution);
  }
}

#endif //PRT_RENDER_FLAGS_H