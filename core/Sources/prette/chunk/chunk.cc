#include "prette/chunk/chunk.h"

#include <functional>
#include <sstream>
#include <string>
#include <utility>

#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/platform.h"
#include "prette/tile.h"
#include "prette/to_string.h"
#include "prette/world/world.h"
#include "prette/world/world_manager.h"

namespace prt {
Chunk::Chunk(const ChunkPos pos) :
  pos_(std::move(pos)) {
  for (uint32_t y = 0; y < kChunkHeight; y++) {
    for (uint32_t x = 0; x < kChunkWidth; x++) {
      TilePos pos(x, y);
      tile(pos) = Tile(this, pos);
    }
  }
}

Chunk::Chunk(const ChunkPos pos, const raw::Chunk& raw) :
  pos_(std::move(pos)) {
  ASSERT(raw.tiles()->size() >= kTotalChunkSize);  // TODO: this probably should be an equals
  for (uint32_t y = 0; y < kChunkHeight; y++) {
    for (uint32_t x = 0; x < kChunkWidth; x++) {
      TilePos pos(x, y);
      const auto index = GetIndex(pos);
      const auto raw_tile = raw.tiles()->Get(index);
      tile(pos) = Tile(this, pos, (*raw_tile));
    }
  }
}

auto Chunk::VisitTiles(std::function<bool(Tile*)> vis) -> bool {
  ChunkTileIterator iter(this);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    if (!vis(next))
      return false;
  }
  return true;
}

auto Chunk::VisitTiles(TileVisitor* vis) -> bool {
  ASSERT(vis);
  ChunkTileIterator iter(this);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    if (!vis->Visit(next))
      return false;
  }
  return true;
}

auto Chunk::ToString() const -> std::string {
  ToStringHelper<Chunk> helper{};
  helper.AddFieldRef("pos", glm::to_string(GetPos()));

  std::stringstream tiles;
  tiles << "[";
  for (auto idx = 0; idx < kTotalChunkSize; idx++) {
    const auto& t = tile(idx);
    tiles << t.ToString();
    if ((idx + 1) < kTotalChunkSize)
      tiles << ", ";
  }
  tiles << "]";
  helper.AddField("tiles", tiles);
  return helper;
}

void Chunk::Save() {
  ASSERT(IsWorldInitialized());
  LOG_IF(FATAL, !GetWorld()->GetStorage()->Save(this)) << "failed to save " << ToString();
}

void Chunk::Load() {}
}  // namespace prt