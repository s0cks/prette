#ifndef PRT_TILE_VERTEX_H
#define PRT_TILE_VERTEX_H

#include "prette/gfx.h"

namespace prt {
struct TileVertex {
  glm::vec2 pos;

  static auto GetBindingDescription() -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(TileVertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static auto GetAttributeDescriptions() -> std::array<VkVertexInputAttributeDescription, 1> {
    std::array<VkVertexInputAttributeDescription, 1> attributes{};
    attributes.at(0).binding = 0;
    attributes.at(0).location = 0;
    attributes.at(0).format = VK_FORMAT_R32G32_SFLOAT;
    attributes.at(0).offset = offsetof(TileVertex, pos);
    return attributes;
  }

  auto operator==(const TileVertex& rhs) const -> bool {
    return pos == rhs.pos;
  }

  auto operator!=(const TileVertex& rhs) const -> bool {
    return pos != rhs.pos;
  }
};
}  // namespace prt

#endif  // PRT_TILE_VERTEX_H
