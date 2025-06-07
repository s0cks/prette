#ifndef PRT_CHUNK_METADATA_H
#define PRT_CHUNK_METADATA_H

#include <array>
#include <cstdint>
#include <type_traits>

#include "prette/glm.h"
#include "prette/std140.h"
#include "prette/tile.h"

namespace prt {
static constexpr const uint32_t kChunkWidth = 32;
static constexpr const uint32_t kChunkHeight = 32;
static constexpr const auto kTotalNumberOfTilesPerChunk = kChunkWidth * kChunkHeight;

using ChunkId = uint32_t;
using ChunkPos = glm::u32vec2;
using ChunkTileData = std::array<Tile, kTotalNumberOfTilesPerChunk>;

struct ChunkData {
  STD140_UINT32(id);
  STD140_U32VEC2(pos);
};

template <>
struct std140::is_aligned<ChunkData> : std::true_type {};
}  // namespace prt

#endif  // PRT_CHUNK_METADATA_H
