#include "prette/world.h"

#include <filesystem>

#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/thread_local.h"
#include "prette/world_event.h"

namespace prt {
DEFINE_string(world_name, "test", "The name of the world.");
DEFINE_string(worlds_dir, "", "The directory to store world data in.");

static WorldEventSubject events_{};

auto OnWorldEvent() -> WorldEventObservable {
  return events_.get_observable();
}

void World::PublishEvent(WorldEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

static inline auto GetWorldName() -> std::string {
  return FLAGS_world_name;
}

static inline auto GetWorldsDir() -> fs::path {
  if (FLAGS_worlds_dir.empty())
    return (fs::current_path() / "worlds");
  return fs::path(FLAGS_worlds_dir);
}

static inline auto GetWorldStorageDir(const std::string& name) -> fs::path {
  return GetWorldsDir() / name;
}

World::World(std::string name) :
  name_(std::move(name)),
  storage_(new WorldStorage(this, GetWorldStorageDir(GetName()))) {}

World::~World() {
  delete storage_;
}

auto World::CreateChunk(const ChunkPos pos) -> Chunk* {
  ASSERT(!Contains(pos));
  if (GetStorage()->Contains(pos)) {
    Chunk* chunk = nullptr;
    LOG_IF(FATAL, !GetStorage()->Load(pos, &chunk)) << "failed to load Chunk at " << glm::to_string(pos) << " from WorldStorage";
    ASSERT(chunk);
    return chunk;
  }
  const auto new_chunk = new Chunk(pos);
  AddChunk(new_chunk);
  return new_chunk;
}

auto World::GetOrCreateChunkAt(const ChunkPos pos) -> Chunk* {
  const auto chunk = chunks_.find(pos);
  if (chunk == std::end(chunks_))
    return CreateChunk(std::move(pos));
  return chunk->second;
}

auto World::VisitChunks(ChunkVisitor* vis) -> bool {
  ASSERT(vis);
  ChunkIterator iter(this);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    ASSERT(next);
    if (!next->Accept(vis))
      return false;
  }
  return true;
}

static ThreadLocal<World> world_{};

static inline auto SetWorld(World* rhs) -> World* {
  ASSERT(rhs);
  world_ = rhs;
  return rhs;
}

auto World::IsInitialized() -> bool {
  return world_.Get() != nullptr;
}

auto World::Get() -> World* {
  ASSERT(IsInitialized());
  return world_;
}

static inline void InitWorldsDir() {
  const auto dir = GetWorldsDir();
  if (!fs::exists(dir)) {
    DVLOG(1) << "created worlds/ directory: " << dir;
    fs::create_directory(dir);
    return;
  }
  LOG_IF(FATAL, !fs::is_directory(dir)) << "cannot create worlds/ because it already exists and isn't a directory: " << dir;
}

void World::InitWorld() {
  ASSERT(!IsInitialized());
  const auto name = GetWorldName();
  DVLOG(1) << "initializing " << name << " World....";
  InitWorldsDir();
  const auto world = SetWorld(World::New(name));
  ASSERT(IsInitialized());
  DVLOG(2) << name << " World initialized.";
  Publish<WorldInitEvent>();
}

void World::DeInitWorld() {
  ASSERT(IsInitialized());
  const auto name = GetWorldName();
  DVLOG(1) << "de-initializing " << name << " World....";
  delete world_.Get();
  world_.Set(nullptr);
  DVLOG(2) << name << " World de-initialized.";
  Publish<WorldDeInitEvent>();
}

void World::Init() {
  OnPostInitEvent().subscribe([](PostInitEvent* event) {
    InitWorld();
  });
  OnTerminatingEvent().subscribe([](TerminatingEvent* event) {
    DeInitWorld();
  });
}
}  // namespace prt