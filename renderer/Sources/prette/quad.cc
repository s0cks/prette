#include "prette/quad.h"

#include "prette/camera.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/pipeline/pipeline.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/vertex/vertex2d.h"

namespace prt {
namespace color2d {
static const QuadVertexArray kQuadVertices = {
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
static const QuadIndexArray kQuadIndices = {
    0, 1, 2, 2, 3, 0,
};

auto NewSingleQuadMesh() -> SingleQuadMesh* {
  return new SingleQuadMesh(kQuadVertices, kQuadIndices);
}

static inline auto CreateGraphicsPipeline() -> vk::RenderPipeline* {
  {
    vk::PipelineLayoutBuilder builder{};
    // clang-format off
    builder.WithName("colored-2d")
      .AddPushConstantRange(0, sizeof(QuadPushData), VK_SHADER_STAGE_VERTEX_BIT);
    // clang-format on
    const vk::PipelineLayout* layout = builder.Build();
    ASSERT_INITIALIZED(layout);
  }
  return vk::RenderPipeline::FromJson("quad-color2d.json");
}

SingleQuadPipeline::SingleQuadPipeline() :
  QuadPipelineTemplate<QuadPushData, SingleQuadMesh>(NewSingleQuadMesh(), CreateGraphicsPipeline()) {
  SetProjection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, kNearClip, kFarClip));
  data_.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

SingleQuadPipeline::~SingleQuadPipeline() = default;
}  // namespace color2d

namespace tex2d {
static const QuadVertexArray kQuadVertices = {
    Vertex{
        .pos = {-1.0f, -1.0f},
        .uv = {0.0f, 0.0f},
    },
    {
        .pos = {1.0f, -1.0f},
        .uv = {0.0f, 0.0f},
    },
    {
        .pos = {1.0f, 1.0f},
        .uv = {0.0f, 0.0f},
    },
    {
        .pos = {-1.0f, 1.0f},
        .uv = {0.0f, 0.0f},
    },
};
static const QuadIndexArray kQuadIndices = {
    0, 1, 2, 2, 3, 0,
};

auto NewSingleQuadMesh() -> SingleQuadMesh* {
  return new SingleQuadMesh(kQuadVertices, kQuadIndices);
}

static inline auto CreateGraphicsPipeline() -> vk::RenderPipeline* {
  return nullptr;
}

SingleQuadPipeline::SingleQuadPipeline() :
  QuadPipelineTemplate<QuadPushData, SingleQuadMesh>(NewSingleQuadMesh(), CreateGraphicsPipeline()) {
  SetProjection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, kNearClip, kFarClip));
}

SingleQuadPipeline::~SingleQuadPipeline() = default;
}  // namespace tex2d
}  // namespace prt