#ifndef PRT_CONFIG_SAVER_H
#define PRT_CONFIG_SAVER_H

#include "prette/assertions.h"
#include "prette/config/config.h"

namespace prt {
class ConfigSaver {
 public:
  ConfigSaver() = default;
  ~ConfigSaver() = default;
  void SaveConfig(Config* config);

 public:
  static inline void Save(Config* rhs) {
    ASSERT(rhs);
    ConfigSaver saver{};
    return saver.SaveConfig(rhs);
  }
};
}  // namespace prt

#endif  // PRT_CONFIG_SAVER_H
