#include "prette/world/world_storage.h"

#include <filesystem>
#include <flatbuffers/flatbuffer_builder.h>
#include <fstream>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/chunk_generated.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/tile.h"

// IWYU pragma: no_include <ios>

namespace prt {
WorldStorage::WorldStorage(World* owner, fs::path path) :
  owner_(owner),
  path_(std::move(path)) {
  if (!fs::exists(GetPath()))
    fs::create_directory(GetPath());
  if (!fs::exists(GetChunksPath()))
    fs::create_directory(GetChunksPath());
}

WorldStorage::~WorldStorage() {
  // do nothing
}

auto WorldStorage::Contains(const ChunkKey k) const -> bool {
  const auto filename = GetChunkPath(k);
  return fs::exists(filename) && fs::is_regular_file(filename);
}

auto WorldStorage::Save(Chunk* chunk) const -> bool {
  ASSERT(chunk);
  flatbuffers::FlatBufferBuilder builder{};
  (*chunk) >> builder;
  const auto filename = GetChunkPath(GetChunkKey(chunk));
  DLOG(INFO) << "saving Chunk at " << glm::to_string(chunk->GetPos()) << " to " << filename << "....";
  std::fstream stream(filename, std::ios::out | std::ios::binary | std::ios::trunc);
  if (!stream.is_open()) {
    LOG(ERROR) << "failed to open " << filename << " for saving " << chunk->ToString();
    return false;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  stream.write((const char*)builder.GetBufferPointer(), builder.GetSize());
  stream.flush();
  stream.close();
  return true;
}

auto WorldStorage::Load(const ChunkKey k, Chunk** result) const -> bool {
  const auto path = GetChunkPath(k);
  DLOG(INFO) << "loading Chunk at " << glm::to_string(k);
  std::fstream stream(path, std::ios::in | std::ios::binary);
  if (!stream.is_open()) {
    LOG(ERROR) << "failed to open " << path;
    return false;
  }
  stream.seekg(0, std::ios::end);
  const auto filesize = stream.tellg();
  stream.seekg(0, std::ios::beg);
  std::vector<uint8_t> buffer{};
  buffer.resize(filesize);
  stream.read((char*)buffer.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  stream.close();
  const auto raw_chunk = raw::GetChunk(buffer.data());
  (*result) = new Chunk(k, (*raw_chunk));
  return true;
}
}  // namespace prt