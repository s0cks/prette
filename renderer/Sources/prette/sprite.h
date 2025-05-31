#ifndef PRT_SPRITE_H
#define PRT_SPRITE_H

#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/descriptor_set.h"
#include "prette/glm.h"
#include "prette/mesh/mesh_class.h"
#include "prette/pipeline/pipeline.h"
#include "prette/relaxed_atomic.h"
#include "prette/std140.h"
#include "prette/texture.h"
#include "prette/vertex/vertex.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vk.h"

namespace prt {
struct SpriteData {
  STD140_MAT4(model) = glm::mat4(1.0f);
  STD140_VEC2(pos) = glm::vec2(0.0f);
  DEFINE_VERTEX_GET_INSTANCE_BINDING_DESCRIPTION(SpriteData);

  static constexpr const auto kTotalNumberOfAttributes = 2;
  using AttributeDescriptions = std::array<VkVertexInputAttributeDescription, kTotalNumberOfAttributes>;
  static auto GetAttributeDescriptions() -> AttributeDescriptions {
    AttributeDescriptions attributes{};
    return attributes;
  }
};

template <>
struct std140::is_aligned<SpriteData> : std::true_type {};

struct SpriteMeshClass : public vk::MeshClass<SpriteData, tex2d::Vertex, 4, uint16_t, 6> {
  static constexpr const auto kClassName = "sprite";
};

static constexpr const auto kDefaultSpriteSheetMaxSize = 65536;
class SpriteRenderer {
  friend class Renderer;
  friend class SceneRenderPass;

 public:
  static constexpr const auto kInitCapacity = 32;

 private:
  vk::RenderPipeline* pipeline_ = nullptr;
  bool first_ = true;
  struct {
    vk::Buffer* buffer = nullptr;
    std::vector<tex2d::Vertex> data{};
  } vertices_;
  struct {
    vk::Buffer* buffer = nullptr;
    std::vector<uint16_t> data{};
  } indices_;
  struct {
    vk::Buffer* buffer = nullptr;
    std::vector<SpriteData> data{};
  } sprites_;
  vk::DescriptorSet* descriptors_ = nullptr;
  Texture* spritesheet_ = nullptr;
  RelaxedAtomic<bool> sprites_changed_ = false;

  auto GetPipeline() const -> vk::RenderPipeline* {
    return pipeline_;
  }

  auto GetVertexBuffer() const -> vk::Buffer* {
    return vertices_.buffer;
  }

  auto GetVertexData() const -> const std::vector<tex2d::Vertex>& {
    return vertices_.data;
  }

  auto GetIndexBuffer() const -> vk::Buffer* {
    return indices_.buffer;
  }

  auto GetIndexData() const -> const std::vector<uint16_t>& {
    return indices_.data;
  }

  auto GetSpritesBuffer() const -> vk::Buffer* {
    return sprites_.buffer;
  }

  auto GetSpriteData() const -> const std::vector<SpriteData>& {
    return sprites_.data;
  }

  auto GetDescriptors() const -> vk::DescriptorSet* {
    return descriptors_;
  }

  auto GetSpriteSheet() const -> Texture* {
    return spritesheet_;
  }

  void AppendSpriteIndices(const uint64_t start);
  auto AppendSpriteVertices(const glm::vec2& min_uv, const glm::vec2& max_uv) -> std::pair<uint64_t, uint64_t>;
  void AddSprite(const glm::vec2 pos, const glm::vec2 min_uv, const glm::vec2 max_uv);

  void UpdateVertexBuffer();
  void UpdateIndexBuffer();
  void UpdateSpritesBuffer();
  void UpdateDescriptors();
  void Render(VkCommandBuffer buffer);

 public:
  SpriteRenderer();
  ~SpriteRenderer();
};
}  // namespace prt

#endif  // PRT_SPRITE_H
