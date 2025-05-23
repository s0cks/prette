#include "prette/quad.h"

#include "prette/descriptor_set.h"
#include "prette/descriptor_set_builder.h"
#include "prette/pipeline.h"
#include "prette/vk.h"

namespace prt {
auto QuadRenderer::CreateColoredDescriptorSet() -> vk::DescriptorSet* {
  vk::DescriptorSetBuilder builder{};
  builder.WithName("colored-quads");
  builder.AddUniformBufferBinding();  // Camera
  return builder;
}

auto QuadRenderer::CreateColoredMesh() -> color2d::QuadMesh* {
  return nullptr;
}

auto QuadRenderer::CreateColoredPipeline() -> vk::RenderPipeline* {
  return vk::RenderPipeline::FromJson("quad-colored");
}

QuadRenderer::QuadRenderer() {
  colored_ = {
      .pipeline = CreateColoredPipeline(),
      .mesh = CreateColoredMesh(),
      .descriptors = CreateColoredDescriptorSet(),
  };
}

QuadRenderer::~QuadRenderer() {}

void RenderAllColored(VkBuffer buffer) {}

void RenderAllTextured(VkBuffer buffer) {}
}  // namespace prt