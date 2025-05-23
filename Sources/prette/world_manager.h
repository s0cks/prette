#ifndef PRT_WORLD_MANAGER_H
#define PRT_WORLD_MANAGER_H

#include <gflags/gflags.h>

#include "prette/common.h"
#include "prette/rx.h"

namespace prt {
DECLARE_string(worlds_dir);

static inline auto GetWorldsDir() -> fs::path {
  if (FLAGS_worlds_dir.empty())
    return (fs::current_path() / "worlds");
  return {FLAGS_worlds_dir};
}

class World;
class WorldManager {
 private:
  rx::subscription on_post_init_{};
  rx::subscription on_terminating_{};

  WorldManager();

  void InitWorld();
  void DeInitWorld();

 public:
  virtual ~WorldManager();

 public:
  static void Init();
};

auto GetWorldManager() -> WorldManager*;

static inline auto IsWorldManagerInitialized() -> bool {
  return GetWorldManager() != nullptr;
}

auto GetWorld() -> World*;
auto IsWorldInitialized() -> bool;

}  // namespace prt

#endif  // PRT_WORLD_MANAGER_H
