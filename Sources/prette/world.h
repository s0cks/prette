#ifndef PRT_WORLD_H
#define PRT_WORLD_H

#include <unordered_map>

#include "prette/chunk.h"
#include "prette/common.h"
#include "prette/flags.h"
#include "prette/world_event.h"
#include "prette/world_storage.h"

namespace prt {
DECLARE_string(world_name);
DECLARE_string(worlds_dir);

auto OnWorldEvent() -> WorldEventObservable;
#define DEFINE_ON_WORLD_EVENT(Name)                                           \
  static inline auto On##Name##Event()->Name##EventObservable {               \
    return OnWorldEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_WORLD_EVENT(DEFINE_ON_WORLD_EVENT)
#undef DEFINE_ON_WORLD_EVENT

class LuaState;
class World {
  friend class Chunk;
  friend class LuaState;
  struct ChunkPosHash {
    auto operator()(const ChunkPos& pos) const -> size_t {
      size_t hash = 0;
      Combine(hash, pos);
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
      return lhs == rhs;
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

 public:
  World(std::string name);
  ~World();

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto AddChunk(Chunk* chunk) -> bool {
    ASSERT(chunk);
    const auto [_, success] = chunks_.insert({chunk->GetPos(), chunk});
    LOG_IF(FATAL, !success) << "failed to register " << chunk->ToString();
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

 private:
  static void InitLua(lua_State* L);
  static void InitWorld();
  static void DeInitWorld();
  static inline auto New(std::string name) -> World* {
    ASSERT(!name.empty());
    return new World(std::move(name));
  }

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> World*;
};
}  // namespace prt

#endif  // PRT_WORLD_H
