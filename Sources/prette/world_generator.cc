#include "prette/world_generator.h"

#include <chrono>
#include <fmt/format.h>
#include <gflags/gflags.h>
#include <string>
#include <units.h>

#include "prette/chunk.h"
#include "prette/common.h"
#include "prette/tile.h"
#include "prette/world.h"

namespace prt {
DEFINE_string(world_name, "test", "The name of the world.");

void WorldInitializer::Append(WorldInitializer** list, WorldInitializer* value) {
  ASSERT(value);
  if ((*list) == nullptr) {
    (*list) = value;
    return;
  }

  auto last = (*list);
  while (last->HasNext())
    last = last->GetNext();
  last->SetNext(value);
}

void WorldInitializer::Remove(WorldInitializer** list, WorldInitializer* value) {
  const auto head = (*list);
  if (!head)
    return;
  if (head == value) {
    (*list) = head->GetNext();
    return;
  }

  auto previous = (*list);
  auto current = (*list)->GetNext();
  while (current && current != value) {
    previous = current;
    current = current->GetNext();
  }

  if (current == nullptr)
    return;
  previous->SetNext(current->GetNext());
}

class TestWorldInitializer : public WorldInitializer {
 public:
  TestWorldInitializer() = default;
  ~TestWorldInitializer() override = default;

  auto GetName() const -> const char* override {
    return "Test";
  }

  auto Apply(World* world) -> bool override {
    const auto chunk = world->GetOrCreateChunkAt(0, 0);
    ASSERT(chunk);
    chunk->GetTileAt(TilePos(0, 0)).SetMaterial(0);
    chunk->GetTileAt(TilePos(kChunkWidth / 2, kChunkHeight / 2)).SetMaterial(0);
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
  const auto name = GetWorldName();
#ifdef PRT_DEBUG
  VLOG(1) << "generating " << name << " World....";
  const auto start = std::chrono::high_resolution_clock::now();
#endif  // PRT_DEBUG
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

#ifdef PRT_DEBUG
  const auto finished = std::chrono::high_resolution_clock::now();
  const auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>((finished - start));
  VLOG(1) << name << " generated in " << units::time::nanosecond_t(static_cast<double>(total_ns.count())) << "!";
#endif  // PRT_DEBUG
  return world;
}
}  // namespace prt