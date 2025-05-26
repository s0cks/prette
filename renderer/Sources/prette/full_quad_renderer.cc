#include "prette/full_quad_renderer.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vulkan/vulkan_core.h>

#include "prette/camera.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_update.h"
#include "prette/pipeline/pipeline.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/thread_local.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
static const FullQuadMesh::VertexArray kTileVertices = {
    color2d::Vertex{
        .pos = {-1.0f, -1.0f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {1.0f, -1.0f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {1.0f, 1.0f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
    {
        .pos = {-1.0f, 1.0f},
        .color = {0.0f, 0.0f, 0.0f, 1.0f},
    },
};
static const FullQuadMesh::IndexArray kTileIndices = {
    0, 1, 2, 2, 3, 0,
};

static inline auto CreateMesh() -> FullQuadMesh* {
  return new FullQuadMesh(kTileVertices, kTileIndices);
}

struct PushData {
  glm::mat4 mvp;
  glm::vec4 color;
};

static inline auto CreatePipeline() -> vk::RenderPipeline* {
  {
    vk::PipelineLayoutBuilder builder{};
    // clang-format off
    builder.WithName("colored-2d")
      .AddPushConstantRange(0, sizeof(PushData), VK_SHADER_STAGE_VERTEX_BIT);
    // clang-format on
    const vk::PipelineLayout* layout = builder.Build();
    ASSERT_INITIALIZED(layout);
  }
  return vk::RenderPipeline::FromJson("full-quad-color2d.json");
}

FullQuadRenderer::FullQuadRenderer() {
  mesh_ = CreateMesh();
  ASSERT_INITIALIZED(mesh_);
  pipeline_ = CreatePipeline();
  ASSERT_INITIALIZED(pipeline_);
}

FullQuadRenderer::~FullQuadRenderer() {
  delete mesh_;
  delete pipeline_;
}

void FullQuadRenderer::Render(VkCommandBuffer buffer) {
  const auto projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, kNearClip, kFarClip);
  const auto view = glm::lookAt(Camera::kDefaultPos, Camera::kDefaultPos + Camera::kDefaultDirection, kWorldUp);
  const auto model = glm::translate(glm::mat4(1.0f), glm::vec3(16.0f, 16.0f, 0.0f));
  PushData push_data{};
  push_data.mvp = projection;
  push_data.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  if (GetMesh()->IsDirty())
    GetMesh()->Sync(true);
  GetPipeline()->Bind(&buffer);
  vkCmdPushConstants(buffer, *GetPipeline()->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData),
                     &push_data);
  GetMesh()->Draw(buffer);
}

static ThreadLocal<FullQuadRenderer> renderer_;

auto IsFullQuadRendererInitialized() -> bool {
  return renderer_.Get() != nullptr;
}

void InitFullQuadRenderer() {
  ASSERT(!IsFullQuadRendererInitialized());
  renderer_ = new FullQuadRenderer();
}
}  // namespace prt