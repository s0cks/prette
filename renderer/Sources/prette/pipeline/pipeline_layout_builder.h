#ifndef PRT_PIPELINE_LAYOUT_BUILDER_H
#define PRT_PIPELINE_LAYOUT_BUILDER_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/vk.h"

namespace prt::vk {
class PipelineLayoutBuilder {
  using VkDescriptorSetLayoutList = std::vector<VkDescriptorSetLayout>;

 private:
  std::string name_{};
  VkPipelineLayoutCreateInfo info_{};
  VkDescriptorSetLayoutList descriptor_set_layouts_{};
  std::vector<VkPushConstantRange> push_ranges_{};

 public:
  PipelineLayoutBuilder() {
    info_.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  }
  ~PipelineLayoutBuilder() = default;

  auto WithName(const std::string name) -> PipelineLayoutBuilder& {
    name_ = std::move(name);
    return *this;
  }

  auto WithDescriptorSetLayouts(const VkDescriptorSetLayout* data, const uint64_t num_layouts)
      -> PipelineLayoutBuilder& {
    for (auto idx = 0; idx < num_layouts; idx++)
      descriptor_set_layouts_.push_back(data[idx]);
    return *this;
  }

  inline auto WithDescriptorSetLayout(const VkDescriptorSetLayout* rhs) -> PipelineLayoutBuilder& {
    ASSERT(rhs);
    return WithDescriptorSetLayouts(rhs, 1);
  }

  inline auto WithDescriptorSetLayout(vk::DescriptorSetLayout* rhs) -> PipelineLayoutBuilder& {
    ASSERT_INITIALIZED(rhs);
    descriptor_set_layouts_.push_back(rhs->GetHandle());
    return *this;
  }

  inline auto WithDescriptorSetLayouts(DescriptorSet* rhs) -> PipelineLayoutBuilder& {
    ASSERT(rhs);
    return WithDescriptorSetLayout(&rhs->GetLayout());
  }

  template <typename DSetLayoutContainer>
  inline auto WithDescriptorSetLayouts(const DSetLayoutContainer& data) -> PipelineLayoutBuilder& {
    ASSERT(!data.empty());
    return WithDescriptorSetLayouts(data.data(), data.size());
  }

  auto AddPushConstantRange(const uint32_t offset, const uint32_t size, const VkShaderStageFlagBits stage)
      -> VkPushConstantRange*;

  auto Build() -> PipelineLayout*;

  auto operator()() -> PipelineLayout* {
    return Build();
  }

  operator PipelineLayout*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_PIPELINE_LAYOUT_BUILDER_H
