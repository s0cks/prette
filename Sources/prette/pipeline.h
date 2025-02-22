#ifndef PRT_PIPELINE_H
#define PRT_PIPELINE_H

#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"

namespace prt {
class GraphicsPipeline {
  DEFINE_DEFAULT_COPYABLE_TYPE(GraphicsPipeline);

 private:
  VkRenderPass pass_ = VK_NULL_HANDLE;
  VkExtent2D extent_{};
  VkPipeline pipeline_ = VK_NULL_HANDLE;
  VkPipelineLayout layout_ = VK_NULL_HANDLE;
  VkPipelineCache cache_ = VK_NULL_HANDLE;

  void Init(const Driver* driver, const std::string& shader, const std::vector<VkDynamicState>& dynamic_states,
            const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const bool depth);

 public:
  GraphicsPipeline() = default;
  explicit GraphicsPipeline(const Driver* driver, const std::string& shader, const VkRenderPass pass,
                            const std::vector<VkDynamicState>& dynamic_states, const VkExtent2D& extent,
                            const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts, const bool depth = false);
  ~GraphicsPipeline() = default;

  auto Get() const -> VkPipeline const& {
    return pipeline_;
  }

  auto GetLayout() const -> VkPipelineLayout const& {
    return layout_;
  }

  auto GetCache() const -> VkPipelineCache const& {
    return cache_;
  }

  void Destroy(const Driver* driver);

  operator VkPipeline() {
    return pipeline_;
  }
};
}  // namespace prt

#endif  // PRT_PIPELINE_H
