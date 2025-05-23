#ifndef PRT_TILE_H
#define PRT_TILE_H

#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

#include "prette/chunk_generated.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/material.h"
#include "prette/mesh.h"
#include "prette/uniform_buffer.h"
#include "prette/vk.h"

namespace prt {
using TilePos = glm::vec2;

struct TileData {
  alignas(8) glm::vec2 pos = glm::vec2(0.0f, 0.0f);
  alignas(4) uint32_t material = 0;
  alignas(16) glm::mat4 model = glm::mat4(1.0f);
  alignas(4) bool hovering = false;

  static inline auto GetBindingDescription(const uint32_t bidx = 0) -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding{};
    binding.binding = bidx;
    binding.stride = sizeof(TileData);
    binding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
    return binding;
  }
};

template <>
struct vk::is_mesh_data_t<TileData> : std::true_type {};

template <>
struct vk::is_uniform_t<TileData> : std::true_type {};

static constexpr const auto kTileSizeInPixels = 32;

class Tile;
class TileVisitor {
 protected:
  TileVisitor() = default;

 public:
  virtual ~TileVisitor() = default;
  virtual auto Visit(Tile* tile) -> bool = 0;
};

static constexpr const auto kDefaultTileMaterial = 1;

class Chunk;
class Tile {
  friend class Chunk;
  DEFINE_DEFAULT_COPYABLE_TYPE(Tile);

 private:
  Chunk* owner_ = nullptr;
  TileData data_{};

 public:
  Tile() = default;
  Tile(Chunk* owner, const TilePos pos, const raw::Tile raw) :
    owner_(owner),
    data_() {
    SetPos(std::move(pos));
    SetMaterial(raw.material());
  }
  Tile(Chunk* owner, const TilePos pos, const MaterialId material = kDefaultTileMaterial) :
    owner_(owner),
    data_() {
    SetPos(pos);
    SetMaterial(material);
  }
  Tile(Chunk* owner, const uint32_t x, const uint32_t y, const MaterialId material) :
    Tile(owner, TilePos(x, y), material) {}
  ~Tile() = default;

  auto GetOwner() const -> Chunk* {
    return owner_;
  }

  inline auto HasOwner() const -> bool {
    return GetOwner() != nullptr;
  }

  auto data() const -> const TileData& {
    return data_;
  }

  auto GetPos() const -> const TilePos& {
    return data().pos;
  }

  void SetPos(const TilePos rhs);

  auto GetMaterial() const -> MaterialId {
    return data().material;
  }

  void SetMaterial(const MaterialId rhs);

  auto Accept(TileVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }

  auto IsHovering() const -> bool {
    return data().hovering;
  }

  void SetHovering(const bool rhs);

  auto GetModel() const -> const glm::mat4& {
    return data().model;
  }

  void SetModel(const glm::mat4 rhs);

  auto Contains(const glm::vec2& pos) const -> bool;
  auto ToString() const -> std::string;

  auto operator=(const raw::Tile& rhs) -> Tile& {
    data_.material = rhs.material();
    return *this;
  }

  friend auto operator<<(std::ostream& stream, const Tile& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt

#endif  // PRT_TILE_H
