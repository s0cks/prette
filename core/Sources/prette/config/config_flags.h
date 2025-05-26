#ifndef PRT_CONFIG_FLAGS_H
#define PRT_CONFIG_FLAGS_H

#include <gflags/gflags.h>

#include "prette/assertions.h"
#include "prette/common.h"

namespace prt {
DECLARE_string(config);
static constexpr const auto kDefaultConfigFilename = "prette.cfg";

static inline auto GetConfigPath() -> fs::path {
  ASSERT(!FLAGS_config.empty());
  return FLAGS_config;
}
}  // namespace prt

#endif  // PRT_CONFIG_FLAGS_H
