#ifndef PRT_CONFIG_LOADER_H
#define PRT_CONFIG_LOADER_H

#include "prette/assertions.h"
#include "prette/config/config.h"

namespace prt {
class ConfigLoader {
 public:
  ConfigLoader() = default;
  ~ConfigLoader() = default;
  void LoadConfig(Config* config);

 public:
  static inline void Load(Config* rhs) {
    ASSERT(rhs);
    ConfigLoader loader{};
    return loader.LoadConfig(rhs);
  }
};
}  // namespace prt

#endif  // PRT_CONFIG_LOADER_H
