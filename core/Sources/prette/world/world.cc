#include "prette/world/world.h"

#include <fmt/format.h>
#include <gflags/gflags.h>
#include <imgui.h>
#include <string>
#include <units.h>
#include <utility>

#include "prette/assertions.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/glm.h"
#include "prette/tile.h"
#include "prette/world/world_event.h"
#include "prette/world/world_manager.h"
#include "prette/world/world_storage.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(WorldEvent, events);

void World::PublishEvent(WorldEvent* event) {
  ASSERT(event);
  const auto& subscriber = events.get_subscriber();
  return subscriber.on_next(event);
}

static inline auto GetWorldStorageDir(const std::string& name) -> fs::path {
  return GetWorldsDir() / name;
}

World::World(std::string name) :
  name_(std::move(name)),
  storage_(new WorldStorage(this, GetWorldStorageDir(GetName()))) {
  ASSERT_NOT_EMPTY(name_);
  ASSERT(storage_);
}

World::~World() {
  delete storage_;
}

auto World::CreateChunk(const ChunkPos pos) -> Chunk* {
  ASSERT(!Contains(pos));
  if (GetStorage()->Contains(pos)) {
    Chunk* chunk = nullptr;
    LOG_IF(FATAL, !GetStorage()->Load(pos, &chunk))
        << "failed to load Chunk at " << glm::to_string(pos) << " from WorldStorage";
    ASSERT(chunk);
    RegisterChunk(chunk);
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
}  // namespace prt