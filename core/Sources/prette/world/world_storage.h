#ifndef PRT_WORLD_STORAGE_H
#define PRT_WORLD_STORAGE_H

#include <filesystem>
#include <fmt/format.h>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/chunk/chunk.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"

namespace prt {
class World;
class WorldStorage {
  friend class World;

 public:
  using ChunkKey = ChunkPos;

 private:
  static inline auto GetChunkKey(Chunk* chunk) -> ChunkKey {
    ASSERT(chunk);
    return chunk->GetPos();
  }

  static inline auto GetChunkFilename(const ChunkKey k) -> std::string {
    return fmt::format("{}{}.dat", k.x, k.y);
  }

 private:
  World* owner_;
  fs::path path_;

  WorldStorage(World* owner, fs::path path);

 public:
  ~WorldStorage();

  auto GetOwner() const -> World* {
    return owner_;
  }

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  inline auto GetChunksPath() const -> fs::path {
    return GetPath() / "chunks";
  }

  auto Contains(const ChunkKey k) const -> bool;
  auto Save(Chunk* chunk) const -> bool;
  auto Load(const ChunkKey pos, Chunk** chunk) const -> bool;

  inline auto GetChunkPath(const ChunkKey k) const -> fs::path {
    return GetChunksPath() / GetChunkFilename(std::move(k));
  }

  inline auto GetChunkPath(Chunk* chunk) const -> fs::path {
    ASSERT(chunk);
    return GetChunkPath(GetChunkKey(chunk));
  }
};
}  // namespace prt

#endif  // PRT_WORLD_STORAGE_H
