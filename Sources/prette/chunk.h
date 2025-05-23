#ifndef PRT_CHUNK_H
#define PRT_CHUNK_H

// IWYU pragma: begin_exports
#include <flatbuffers/flatbuffer_builder.h>

#include "prette/chunk_generated.h"
// IWYU pragma: end_exports

#include <algorithm>
#include <array>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "prette/common.h"
#include "prette/glm.h"
#include "prette/relaxed_atomic.h"
#include "prette/tile.h"
#include "prette/to_string.h"
#include "prette/uniform_buffer.h"

namespace prt {
static constexpr const uint32_t kChunkWidth = 32;
static constexpr const uint32_t kChunkHeight = 32;
static constexpr const auto kTotalChunkSize = kChunkWidth * kChunkHeight;

using ChunkId = uint32_t;
using ChunkPos = glm::u32vec2;
using ChunkData = std::array<Tile, kTotalChunkSize>;

class Chunk;
class ChunkVisitor {
 protected:
  ChunkVisitor() = default;

 public:
  virtual ~ChunkVisitor() = default;
  virtual auto Visit(Chunk* chunk) -> bool = 0;
};

struct ChunkMetadata {
  alignas(4) ChunkId id{};
  alignas(8) ChunkPos pos{};
};

template <>
struct vk::is_uniform_t<ChunkMetadata> : std::true_type {};

using ChunkMetadataBuffer = vk::UniformBuffer<ChunkMetadata, 1>;

class Chunk {
  friend class Tile;
  friend class World;
  using ChunkIndex = uint32_t;
  friend class ChunkTileIterator;
  DEFINE_DEFAULT_COPYABLE_TYPE(Chunk);

 public:
  class ChunkTileIterator {
   private:
    Chunk* chunk_;
    ChunkIndex current_ = 0;

   public:
    explicit ChunkTileIterator(Chunk* chunk) :
      chunk_(chunk) {}
    ~ChunkTileIterator() = default;

    auto HasNext() const -> bool {
      return current_ < kTotalChunkSize;
    }

    auto Next() -> Tile* {
      auto next = &chunk_->tile(current_);
      current_ += 1;
      return next;
    }
  };

 private:
  static inline auto GetIndex(const TilePos& pos) -> ChunkIndex {
    return static_cast<uint32_t>(pos.y) * kChunkWidth + static_cast<uint32_t>(pos.x);
  }

  static inline auto GetChunkPos(const ChunkIndex index) -> ChunkPos {
    return {index % kChunkWidth, index / kChunkHeight};
  }

 private:
  RelaxedAtomic<bool> dirty_ = true;
  ChunkPos pos_{};
  ChunkData tiles_{};

  inline auto data() -> ChunkData& {
    return tiles_;
  }

  inline auto tile(const ChunkIndex idx) -> Tile& {
    ASSERT(idx >= 0 && idx <= tiles_.size());
    return tiles_.at(idx);
  }

  inline auto tile(const TilePos pos) -> Tile& {
    return tile(GetIndex(pos));
  }

  inline auto tile(const ChunkIndex idx) const -> const Tile& {
    ASSERT(idx >= 0 && idx <= tiles_.size());
    return tiles_.at(idx);
  }

  inline auto tile(const TilePos pos) const -> const Tile& {
    return tile(GetIndex(pos));
  }

 public:
  Chunk() = default;
  explicit Chunk(const ChunkPos pos);
  Chunk(const uint32_t x, const uint32_t y) :
    Chunk(ChunkPos(x, y)) {}
  Chunk(const ChunkPos pos, const raw::Chunk& raw);
  ~Chunk() = default;

  auto IsDirty() const -> bool {
    return (bool)dirty_;
  }

  auto GetTiles() const -> const ChunkData& {
    return tiles_;
  }

  auto GetPos() const -> const ChunkPos& {
    return pos_;
  }

  auto GetTileAt(const TilePos pos) -> Tile& {
    return tile(std::move(pos));
  }

  auto GetTileAt(const TilePos pos) const -> const Tile& {
    return tile(std::move(pos));
  }

  void SetTileAt(const TilePos pos, const raw::Tile& rhs) {
    tile(pos) = rhs;
    Save();
  }

  void SetTileAt(const uint32_t x, const uint32_t y, const raw::Tile& rhs) {
    return SetTileAt(TilePos(x, y), rhs);
  }

  auto Accept(ChunkVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }

  auto VisitTiles(TileVisitor* vis) -> bool;
  auto VisitTiles(std::function<bool(Tile*)> vis) -> bool;
  auto ToString() const -> std::string;
  void Save();
  void Load();

  void SetDirty(const bool rhs) {
    dirty_ = rhs;
  }

  inline void MarkDirty() {
    return SetDirty(true);
  }

  inline void UnmarkDirty() {
    return SetDirty(false);
  }

  auto operator[](const TilePos pos) const -> const Tile& {
    return GetTileAt(pos);
  }

  auto operator<<(std::vector<raw::Tile>& tiles) -> Chunk& {
    ASSERT(tiles.size() == kTotalChunkSize);
    for (ChunkIndex idx = 0; idx < kTotalChunkSize; idx++) {
      tile(idx) = Tile(this, GetChunkPos(idx), tiles[idx]);
    }
    MarkDirty();
    return *this;
  }

  auto operator>>(flatbuffers::FlatBufferBuilder& builder) -> flatbuffers::FlatBufferBuilder& {
    std::vector<raw::Tile> tiles{};
    std::ranges::for_each(tiles_, [&tiles](const Tile& tile) {
      tiles.emplace_back(tile.GetMaterial());
    });
    const auto tiles_vector = builder.CreateVectorOfStructs(tiles);
    raw::ChunkBuilder chunk_builder(builder);
    chunk_builder.add_tiles(tiles_vector);
    const auto raw_chunk = chunk_builder.Finish();
    builder.Finish(raw_chunk);
    return builder;
  }

  friend auto operator<<(std::ostream& stream, const Chunk& rhs) -> std::ostream& {
    ToStringHelper<Chunk> helper{};
    helper.AddFieldRef("pos", glm::to_string(rhs.GetPos()));
    return stream << helper;
  }
};
}  // namespace prt

#endif  // PRT_CHUNK_H
