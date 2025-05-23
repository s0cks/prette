#include "prette/world_manager.h"

#include <gflags/gflags.h>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/thread_local.h"
#include "prette/world.h"
#include "prette/world_generator.h"

namespace prt {
DEFINE_string(worlds_dir, "", "The directory to store world data in.");

static ThreadLocal<WorldManager> manager_{};
static ThreadLocal<World> world_{};

static inline auto SetWorld(World* rhs) -> World* {
  ASSERT(rhs);
  world_ = rhs;
  return rhs;
}

static inline void InitWorldsDir() {
  const auto dir = GetWorldsDir();
  if (!fs::exists(dir)) {
    DVLOG(1) << "created worlds/ directory: " << dir;
    fs::create_directory(dir);
    return;
  }
  LOG_IF(FATAL, !fs::is_directory(dir)) << "cannot create worlds/ because it already exists and isn't a "
                                           "directory: "
                                        << dir;
}

auto GetWorld() -> World* {
  ASSERT(IsWorldInitialized());
  return world_;
}

auto IsWorldInitialized() -> bool {
  return world_.Get() != nullptr;
}

auto GetWorldManager() -> WorldManager* {
  return manager_;
}

void WorldManager::InitWorld() {
  ASSERT(!IsWorldInitialized());
  InitWorldsDir();
  DefaultWorldGenerator generator{};
  const auto world = SetWorld(generator.Generate());
  ASSERT(IsWorldInitialized());
  World::PublishWorldCreatedEvent();
}

void WorldManager::DeInitWorld() {
  ASSERT(IsWorldInitialized());
  const auto name = GetWorldName();
  DVLOG(1) << "de-initializing " << name << " World....";
  world_.Release();
  DVLOG(2) << name << " World de-initialized.";
  ASSERT(!IsWorldInitialized());
  World::PublishWorldDestroyedEvent();
}

WorldManager::WorldManager() {
  on_post_init_ = OnPostInitEvent().subscribe([this](PostInitEvent* event) {
    InitWorld();
  });
  on_terminating_ = OnTerminatingEvent().subscribe([this](TerminatingEvent* event) {
    DeInitWorld();
  });
}

WorldManager::~WorldManager() {
  on_post_init_.unsubscribe();
  on_terminating_.unsubscribe();
}

void WorldManager::Init() {
  ASSERT(!IsWorldManagerInitialized());
  manager_ = new WorldManager();
  ASSERT(IsWorldManagerInitialized());
}

}  // namespace prt