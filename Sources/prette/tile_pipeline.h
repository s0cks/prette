#ifndef PRT_TILE_PIPELINE_H
#define PRT_TILE_PIPELINE_H

#include "prette/pipeline.h"
#include "vulkan/vulkan_core.h"

namespace prt {
class TileGraphicsPipeline;
class TileGraphicsPipelineBuilder : public TemplateRenderPipelineBuilder<TileGraphicsPipeline> {
 public:
  explicit TileGraphicsPipelineBuilder(const VkExtent2D extent) :
    TemplateRenderPipelineBuilder<TileGraphicsPipeline>(extent) {}
  ~TileGraphicsPipelineBuilder() override = default;
  auto Build() -> TileGraphicsPipeline* override;
};

class TileGraphicsPipeline : public BaseRenderPipeline {
  friend class TileGraphicsPipelineBuilder;

 private:
  TileGraphicsPipeline(VkRenderPass render_pass, const VkExtent2D& extent, VkPipeline pipeline, VkPipelineLayout layout,
                       VkPipelineCache cache) :
    BaseRenderPipeline(render_pass, extent, pipeline, layout, cache) {}

 public:
  ~TileGraphicsPipeline() = default;
};
}  // namespace prt

#endif  // PRT_TILE_PIPELINE_H
