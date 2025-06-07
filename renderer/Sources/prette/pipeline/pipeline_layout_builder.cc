#include "prette/pipeline/pipeline_layout_builder.h"

#include <cstdint>

#include "prette/assertions.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/vk.h"

namespace prt::vk {
auto PipelineLayoutBuilder::AddPushConstantRange(const uint32_t offset, const uint32_t size,
                                                 const VkShaderStageFlagBits stage) -> VkPushConstantRange* {
  const auto idx = push_ranges_.size();
  push_ranges_.resize(idx + 1);
  push_ranges_[idx] = VkPushConstantRange{
      .stageFlags = stage,
      .offset = offset,
      .size = size,
  };
  return &push_ranges_[idx];
}

auto PipelineLayoutBuilder::Build() -> PipelineLayout* {
  ASSERT(!name_.empty());
  info_.setLayoutCount = descriptor_set_layouts_.size();
  info_.pSetLayouts = descriptor_set_layouts_.data();
  info_.pushConstantRangeCount = push_ranges_.size();
  info_.pPushConstantRanges = push_ranges_.data();
  return PipelineLayout::New(name_, info_);
}
}  // namespace prt::vk