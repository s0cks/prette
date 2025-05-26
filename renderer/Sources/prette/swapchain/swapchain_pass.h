#ifndef PRT_SWAPCHAIN_PASS_H
#define PRT_SWAPCHAIN_PASS_H

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/descriptor_set.h"
#include "prette/render_pass/render_pass.h"
#include "prette/vk.h"

namespace prt::vk {
class SwapchainRenderPass : public RenderPass {
  static auto CreateDescriptors() -> vk::DescriptorSet*;
  static void CreatePipelineLayout(vk::DescriptorSet* descriptors);

 protected:
  void UpdateDescriptors() override;

 public:
  SwapchainRenderPass(const std::string name, VkRenderPassCreateInfo* create_info);
  ~SwapchainRenderPass() override = default;
  void Execute() override;

 public:
  static auto New() -> SwapchainRenderPass*;
};
}  // namespace prt::vk

#endif  // PRT_SWAPCHAIN_PASS_H
