#ifndef PRT_QUAD_H
#define PRT_QUAD_H

#include "prette/descriptor_set.h"
#include "prette/index_class.h"
#include "prette/mesh.h"
#include "prette/mesh_class.h"
#include "prette/pipeline.h"
#include "prette/platform.h"
#include "prette/transform.h"
#include "prette/vertex.h"
#include "prette/vertex2d.h"
#include "prette/vk.h"

namespace prt {
static constexpr const auto kMaxNumberOfQuadsPerInstancedMesh = 32;
static constexpr const auto kTotalNumberOfVerticesPerQuad = 4;
static constexpr const auto kTotalNumberOfIndicesPerQuad = 6;
#define DEFAULT_QUAD_INDEX_TYPE uint8_t

template <typename D, VertexType2D V, vk::IndexType Index = DEFAULT_QUAD_INDEX_TYPE>
struct QuadMeshClass :
  public vk::MeshClass<D, V, kTotalNumberOfVerticesPerQuad, Index, kTotalNumberOfIndicesPerQuad> {};

namespace color2d {
struct Quad {
  Transform transform{};
};

struct QuadMeshClass : public prt::QuadMeshClass<Quad, Vertex, DEFAULT_QUAD_INDEX_TYPE> {
  static constexpr const auto kClassName = "quad-color2d";
};

using QuadMesh = vk::InstancedMesh<QuadMeshClass, kMaxNumberOfQuadsPerInstancedMesh>;
}  // namespace color2d

namespace mat2d {
struct Quad {
  Transform transform{};
};

struct QuadMeshClass : public prt::QuadMeshClass<Quad, Vertex, DEFAULT_QUAD_INDEX_TYPE> {
  static constexpr const auto kClassName = "quad-mat2d";
};

using QuadMesh = vk::InstancedMesh<QuadMeshClass, kMaxNumberOfQuadsPerInstancedMesh>;
}  // namespace mat2d

namespace tex2d {
struct Quad {
  Transform transform{};
  alignas(8) uint64_t texture = 0;
};

struct QuadMeshClass : public prt::QuadMeshClass<Quad, Vertex, DEFAULT_QUAD_INDEX_TYPE> {
  static constexpr const auto kClassName = "quad-tex2d";
};

using QuadMesh = vk::InstancedMesh<QuadMeshClass, kMaxNumberOfQuadsPerInstancedMesh>;
}  // namespace tex2d

class QuadRenderer {
 private:
  struct {
    vk::RenderPipeline* pipeline = nullptr;
    color2d::QuadMesh* mesh = nullptr;
    vk::DescriptorSet* descriptors = nullptr;
  } colored_;
  struct {
    vk::RenderPipeline* pipeline = nullptr;
    color2d::QuadMesh* mesh = nullptr;
    vk::DescriptorSet* descriptors = nullptr;
  } textured_;

  static auto CreateColoredDescriptorSet() -> vk::DescriptorSet*;
  static auto CreateColoredMesh() -> color2d::QuadMesh*;
  static auto CreateColoredPipeline() -> vk::RenderPipeline*;

  inline auto GetColoredPipeline() const -> vk::RenderPipeline* {
    return colored_.pipeline;
  }

  inline auto GetColoredMesh() const -> color2d::QuadMesh* {
    return colored_.mesh;
  }

  inline auto GetColoredDescriptors() const -> vk::DescriptorSet* {
    return colored_.descriptors;
  }

  static auto CreateTexturedDescriptorSet() -> vk::DescriptorSet*;
  static auto CreateTexturedMesh() -> color2d::QuadMesh*;
  static auto CreateTexturedPipeline() -> vk::RenderPipeline*;

  inline auto GetTexturedPipeline() const -> vk::RenderPipeline* {
    return textured_.pipeline;
  }

  inline auto GetTexturedMesh() const -> color2d::QuadMesh* {
    return textured_.mesh;
  }

  inline auto GetTexturedDescriptors() const -> vk::DescriptorSet* {
    return textured_.descriptors;
  }

 public:
  QuadRenderer();
  ~QuadRenderer();

  void RenderAllColored(VkBuffer buffer);
  void RenderAllTextured(VkBuffer buffer);

  inline void RenderAll(VkBuffer buffer) {
    RenderAllColored(buffer);
    RenderAllTextured(buffer);
  }
};
}  // namespace prt

#endif  // PRT_QUAD_H
