#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <functional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/pipeline/pipeline_builder.h"
#include "prette/pipeline/pipeline_cache.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/vk.h"

namespace prt::vk {
static inline void InitPipelineViewportState(VkPipelineViewportStateCreateInfo& create_info,
                                             const std::vector<VkViewport>& viewports,
                                             const std::vector<VkRect2D>& scissors) {
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  create_info.viewportCount = viewports.size();
  create_info.pViewports = viewports.data();
  create_info.scissorCount = scissors.size();
  create_info.pScissors = scissors.data();
}

class RenderPipeline;
using RenderPipelinePredicate = std::function<bool(RenderPipeline*)>;

class RenderPipeline : public vk::NamedHandleTemplate<VkPipeline> {
 public:
  static constexpr const VkExtent2D kDefaultExtent = {0, 0};

 protected:
  vk::PipelineLayout* layout_;
  vk::PipelineCache* cache_;
  VkExtent2D extent_;

 public:
  RenderPipeline(std::string name, const VkExtent2D& extent, const VkGraphicsPipelineCreateInfo& create_info,
                 vk::PipelineLayout* layout, vk::PipelineCache* cache);
  ~RenderPipeline() override;

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  inline auto HasExtent() const -> bool {
    return GetExtent() != kDefaultExtent;
  }

  auto GetPipelineLayout() const -> vk::PipelineLayout* {
    return layout_;
  }

  auto GetPipelineCache() const -> vk::PipelineCache* {
    return cache_;
  }

  auto ToString() const -> std::string override;
  void Bind(VkCommandBuffer* buffer, const VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

  operator VkPipeline() const {
    return GetHandle();
  }

 public:
  static auto FromJson(const std::string name) -> RenderPipeline*;
};

class RenderPipeline;
class RenderPipelineBuilder : public TemplateRenderPipelineBuilder<RenderPipeline> {
 public:
  explicit RenderPipelineBuilder(const VkExtent2D extent = {}) :
    TemplateRenderPipelineBuilder<RenderPipeline>(extent) {}
  ~RenderPipelineBuilder() override = default;

  auto Build() -> RenderPipeline* override;

  operator VkGraphicsPipelineCreateInfo() const {
    return pipeline_;
  }
};

class RenderPipelineVisitor {
 protected:
  RenderPipelineVisitor() = default;

 public:
  virtual ~RenderPipelineVisitor() = default;
  virtual auto Visit(RenderPipeline* rhs) -> bool = 0;
};

auto VisitAllRenderPipelines(RenderPipelineVisitor* vis) -> bool;
auto VisitAllRenderPipelines(RenderPipelinePredicate vis) -> bool;
auto GetNumberOfRenderPipelines() -> uint64_t;

class RenderPipelineFinalizer : public RenderPipelineVisitor {
 private:
  uint64_t num_finalized_ = 0;

 public:
  RenderPipelineFinalizer() = default;
  ~RenderPipelineFinalizer() override = default;
  auto Visit(RenderPipeline* rhs) -> bool override;

  auto GetNumberOfObjectsFinalized() const -> uint64_t {
    return num_finalized_;
  }

  auto operator()(RenderPipeline* rhs) -> bool {
    return Visit(rhs);
  }

 public:
  static void FinalizeAll();
  static void FinalizeAll(const std::vector<RenderPipeline*>& rhs);
};
}  // namespace prt::vk

#endif  // PRT_PIPELINE_H
