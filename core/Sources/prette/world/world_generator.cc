#include "prette/world/world_generator.h"

#include <chrono>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <string>
#include <units.h>

#include "prette/assertions.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/tile.h"
#include "prette/world/world.h"

namespace prt {
DEFINE_string(world_name, "test", "The name of the world.");

class TestWorldInitializer : public WorldInitializer {
 public:
  TestWorldInitializer() = default;
  ~TestWorldInitializer() override = default;

  auto GetName() const -> const char* override {
    return "Test";
  }

  auto Apply(World* world) -> bool override {
    static constexpr const auto kInitChunkRadius = 4;
    for (auto x = -kInitChunkRadius; x < 1 + (kInitChunkRadius * 2); x++) {
      for (auto y = -kInitChunkRadius; y < 1 + (kInitChunkRadius * 2); y++) {
        const auto chunk_pos = ChunkPos(x, y);
        const auto c = world->GetOrCreateChunkAt(chunk_pos);
        ASSERT(c);
        if (chunk_pos == ChunkPos(0)) {
          c->GetTileAt(TilePos(0, 0)).SetMaterial(0);
          c->GetTileAt(TilePos(kChunkWidth / 2, kChunkHeight / 2)).SetMaterial(0);
        }
      }
    }
    return true;
  }
};

DefaultWorldGenerator::DefaultWorldGenerator() :
  WorldGenerator() {
  AddInitializer(new TestWorldInitializer());
}

DefaultWorldGenerator::~DefaultWorldGenerator() {
  auto current = GetInitializerList();
  while (current && current->HasNext()) {
    const auto next = current->GetNext();
    delete current;
    current = next;
  }
  delete current;
}

auto DefaultWorldGenerator::Generate() -> World* {
  const auto& name = GetWorldName();
  DVLOG(2) << "generating " << name << " world....";
  PRT_PROFILING_BEGIN(world_gen)
  const auto world = World::New(name);
  ASSERT(world);
  WorldInitializerIterator iter(GetInitializerList());
  while (iter.HasNext()) {
    const auto next = iter.Next();
    if (!next->Apply(world)) {
      delete world;
      throw WorldGenerationException(fmt::format("failed to apply `{0:s}` WorldInitializer", next->GetName()));
    }
  }
  PRT_PROFILING_END(world_gen)
  DVLOG(1) << name
           << " world generated in: " << std::chrono::duration_cast<std::chrono::milliseconds>(world_gen_duration);
  return world;
}
}  // namespace prt