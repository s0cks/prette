#ifndef PRT_PIPELINE_BUILDER_H
#define PRT_PIPELINE_BUILDER_H

#include <utility>

#include "prette/gfx.h"
#include "prette/pipeline.h"
#include "prette/shader.h"
#include "vulkan/vulkan_core.h"

namespace prt {
class PipelineCacheBuilder {
 public:
  PipelineCacheBuilder() = default;
  ~PipelineCacheBuilder() = default;

  void Build(VkPipelineCache& cache);
};

class PipelineLayoutBuilder {
  using VkDescriptorSetLayoutList = std::vector<VkDescriptorSetLayout>;

 private:
  VkDescriptorSetLayoutList descriptor_set_layouts_{};

 public:
  explicit PipelineLayoutBuilder(VkDescriptorSetLayoutList descriptor_set_layouts) :
    descriptor_set_layouts_(std::move(descriptor_set_layouts)) {}
  ~PipelineLayoutBuilder() = default;

  void Build(VkPipelineLayout& layout);
};

class Shader;
struct GraphicsPipelineBuilder : public TemplateRenderPipelineBuilder<GraphicsPipeline> {
  explicit GraphicsPipelineBuilder(const VkExtent2D extent) :
    TemplateRenderPipelineBuilder<GraphicsPipeline>(std::move(extent)) {}
  ~GraphicsPipelineBuilder() = default;
  auto Build() -> GraphicsPipeline* override;
};
}  // namespace prt

#endif  // PRT_PIPELINE_BUILDER_H
