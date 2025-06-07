#ifndef PRT_WORLD_H
#define PRT_WORLD_H

#include <functional>
#include <gflags/gflags.h>
#include <string>
#include <unordered_map>
#include <utility>

#include "prette/assertions.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/rx.h"
#include "prette/world/world_event.h"
#include "prette/world/world_manager.h"
#include "prette/world/world_state.h"
#include "prette/world/world_storage.h"

namespace prt {
DECLARE_string(worlds_dir);

auto GetWorldEventObservable() -> WorldEventObservable;

template <typename... ArgN>
static inline auto OnWorldEvent(ArgN... args) -> rx::composite_subscription {
  return GetWorldEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                            \
  template <typename... ArgN>                                                            \
  static inline auto On##Name##Event()->Name##EventObservable {                          \
    return GetWorldEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                      \
  template <typename... ArgN>                                                            \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {                \
    return On##Name##Event().subscribe(args...);                                         \
  }
FOR_EACH_WORLD_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_WORLD_EVENT
#undef DEFINE_ON_EVENT

class World {
  friend class Chunk;
  friend class WorldManager;
  struct ChunkPosHash {
    auto operator()(const ChunkPos& pos) const -> size_t {
      size_t hash = 0;
      Combine(hash, pos[0]);
      Combine(hash, pos[1]);
      return hash;
    }

   private:
    template <class T>
    static inline void Combine(std::size_t& seed, const T& v) {
      std::hash<T> hasher;
      seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  };

  struct ChunkPosEq {
    auto operator()(const ChunkPos& lhs, const ChunkPos& rhs) const -> bool {
      return lhs.x == rhs.x && lhs.y == rhs.y;
    }
  };

  using ChunkMap = std::unordered_map<ChunkPos, Chunk*, ChunkPosHash, ChunkPosEq>;
  DEFINE_NON_COPYABLE_TYPE(World);

 public:
  class ChunkIterator {
   private:
    World* world_;
    ChunkMap::iterator current_;

   public:
    explicit ChunkIterator(World* world) :
      world_(world),
      current_(std::begin(world_->chunks_)) {}
    ~ChunkIterator() = default;

    auto GetWorld() const -> World* {
      return world_;
    }

    auto HasNext() const -> bool {
      return current_ != std::end(GetWorld()->chunks());
    }

    auto Next() -> Chunk* {
      const auto [pos, chunk] = (*current_);
      current_++;
      return chunk;
    }
  };

 private:
  std::string name_;
  ChunkMap chunks_{};
  WorldStorage* storage_;

  inline auto chunks() -> ChunkMap& {
    return chunks_;
  }

  inline auto chunks() const -> const ChunkMap& {
    return chunks_;
  }

  auto GetStorage() const -> WorldStorage* {
    return storage_;
  }

  auto CreateChunk(const ChunkPos pos) -> Chunk*;

 private:
  static void PublishEvent(WorldEvent* event);

  template <typename E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

  void RegisterChunk(Chunk* chunk) {
    const auto [_, success] = chunks_.insert({chunk->GetPos(), chunk});
    LOG_IF(FATAL, !success) << "failed to register " << chunk->ToString();
  }

#define DEFINE_PUBLISH_EVENT(Name)            \
  static inline void Publish##Name##Event() { \
    return Publish<Name##Event>();            \
  }
  FOR_EACH_WORLD_STATE(DEFINE_PUBLISH_EVENT)
#undef DEFINE_PUBLISH_EVENT

 public:
  World(std::string name);
  ~World();

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto AddChunk(Chunk* chunk) -> bool {
    ASSERT(chunk);
    RegisterChunk(chunk);
    LOG_IF(FATAL, !GetStorage()->Save(chunk)) << "failed to save " << chunk->ToString();
    return true;
  }

  auto HasChunk(const ChunkPos pos) const -> bool {
    const auto chunk = chunks_.find(pos);
    return chunk != std::end(chunks_);
  }

  auto GetChunkAt(const ChunkPos pos) const -> Chunk* {
    const auto chunk = chunks_.find(pos);
    return chunk != std::end(chunks_) ? chunk->second : nullptr;
  }

  auto Contains(const ChunkPos pos) const -> bool {
    const auto chunk = chunks_.find(pos);
    return chunk != std::end(chunks_);
  }

  auto GetOrCreateChunkAt(const ChunkPos pos) -> Chunk*;

  inline auto GetOrCreateChunkAt(const uint32_t x, const uint32_t y) -> Chunk* {
    return GetOrCreateChunkAt(ChunkPos(x, y));
  }

  auto VisitChunks(ChunkVisitor* vis) -> bool;
  auto VisitChunksAround(const ChunkPos pos, const uint32_t radius, ChunkVisitor* vis) -> bool;
  auto VisitChunksAround(const ChunkPos pos, const uint32_t radius, std::function<bool(Chunk*)> vis) -> bool;

 public:
  static inline auto New(std::string name) -> World* {
    ASSERT_NOT_EMPTY(name);
    return new World(std::move(name));
  }
};
}  // namespace prt

#endif  // PRT_WORLD_H
