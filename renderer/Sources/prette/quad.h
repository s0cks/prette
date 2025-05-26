#ifndef PRT_QUAD_H
#define PRT_QUAD_H

#include <array>
#include <utility>

#include "prette/descriptor_set.h"
#include "prette/index_class.h"
#include "prette/mesh.h"
#include "prette/mesh_class.h"
#include "prette/pipeline/pipeline.h"
#include "prette/platform.h"
#include "prette/vertex/vertex.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vk.h"

namespace prt {
struct QuadData {};

static constexpr const auto kMaxNumberOfQuadsPerInstancedMesh = 32;
static constexpr const auto kTotalNumberOfVerticesPerQuad = 4;
static constexpr const auto kTotalNumberOfIndicesPerQuad = 6;
#define DEFAULT_QUAD_INDEX_TYPE uint16_t

template <VertexType2D V, vk::IndexType Index = DEFAULT_QUAD_INDEX_TYPE>
struct BaseQuadMeshClass :
  public vk::MeshClass<QuadData, V, kTotalNumberOfVerticesPerQuad, Index, kTotalNumberOfIndicesPerQuad> {
  static constexpr const auto kIsQuadMesh = true;
};

template <typename T>
concept PushDataType = requires(T value) {
  { value.projection };
};

template <typename T>
concept QuadMeshType = requires {
  { T::MeshClass::kIsQuadMesh };
};

class BaseQuadPipeline {
 protected:
  BaseQuadPipeline() = default;

  virtual void SyncMesh() = 0;
  virtual void BindPipeline(VkCommandBuffer* buffer) = 0;
  virtual void DrawMesh(VkCommandBuffer* buffer) = 0;

 public:
  virtual ~BaseQuadPipeline() = default;

  virtual void Render(VkCommandBuffer* buffer) {
    SyncMesh();
    BindPipeline(buffer);
    DrawMesh(buffer);
  }
};

template <PushDataType PushData, QuadMeshType Mesh>
class QuadPipelineTemplate : public BaseQuadPipeline {
 private:
  Mesh* mesh_ = nullptr;
  vk::RenderPipeline* pipeline_;

 protected:
  PushData data_{};

 protected:
  QuadPipelineTemplate(Mesh* mesh, vk::RenderPipeline* pipeline) :
    BaseQuadPipeline(),
    mesh_(mesh),
    pipeline_(pipeline) {}

  void SyncMesh() override {
    if (GetMesh()->IsDirty())
      GetMesh()->Sync(true);
  }

  void BindPipeline(VkCommandBuffer* buffer) override {
    GetPipeline()->Bind(buffer);
  }

  void DrawMesh(VkCommandBuffer* buffer) override {
    vkCmdPushConstants(*buffer, *GetPipeline()->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData),
                       &data_);
    GetMesh()->Draw(*buffer);
  }

 public:
  virtual ~QuadPipelineTemplate() = default;

  auto GetMesh() const -> Mesh* {
    return mesh_;
  }

  auto GetPipeline() const -> vk::RenderPipeline* {
    return pipeline_;
  }

  auto GetPushData() const -> const PushData& {
    return data_;
  }

  void SetPushData(const PushData rhs) {
    data_ = std::move(rhs);
  }

  void SetProjection(const glm::mat4 rhs) {
    data_.projection = std::move(rhs);
  }

  auto IsInitialized() const -> bool {
    return vk::AllInitialized(pipeline_) && mesh_;
  }
};

namespace color2d {
struct QuadMeshClass : public prt::BaseQuadMeshClass<Vertex> {
  static constexpr const auto kClassName = "quad-color2d";
};

using QuadVertexArray = std::array<Vertex, kTotalNumberOfVerticesPerQuad>;
using QuadIndexArray = std::array<DEFAULT_QUAD_INDEX_TYPE, kTotalNumberOfIndicesPerQuad>;

struct QuadPushData {
  glm::mat4 projection;
  glm::vec4 color;
};

using SingleQuadMesh = vk::SingleMesh<QuadMeshClass>;
class SingleQuadPipeline : public QuadPipelineTemplate<QuadPushData, SingleQuadMesh> {
 public:
  SingleQuadPipeline();
  ~SingleQuadPipeline() override;
};
}  // namespace color2d

namespace tex2d {
struct QuadMeshClass : public prt::BaseQuadMeshClass<Vertex> {
  static constexpr const auto kClassName = "quad-tex2d";
};

using QuadVertexArray = std::array<Vertex, kTotalNumberOfVerticesPerQuad>;
using QuadIndexArray = std::array<DEFAULT_QUAD_INDEX_TYPE, kTotalNumberOfIndicesPerQuad>;

struct QuadPushData {
  glm::mat4 projection;
};

using SingleQuadMesh = vk::SingleMesh<QuadMeshClass>;
class SingleQuadPipeline : public QuadPipelineTemplate<QuadPushData, SingleQuadMesh> {
 public:
  SingleQuadPipeline();
  ~SingleQuadPipeline() override;
};
}  // namespace tex2d
}  // namespace prt

#endif  // PRT_QUAD_H
