#include "prette/descriptor_set_layout.h"

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/gfx_vk.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* create_info) :
  vk::HandleTemplate<VkDescriptorSetLayout>() {
  const auto driver = Driver::Get();
  driver->CreateDescriptorSetLayout(create_info, handle_ptr());
}

DescriptorSetLayout::~DescriptorSetLayout() {
  const auto driver = Driver::Get();
  driver->DestroyDescriptorSetLayout(handle_ref());
}

auto DescriptorSetLayout::ToString() const -> std::string {
  ToStringHelper<DescriptorSetLayout> helper{};
  return helper;
}

auto DescriptorSetLayoutBuilder::Build() -> DescriptorSetLayout* {
  ASSERT(IsValid());
  info_ptr()->bindingCount = bindings_.size();
  info_ptr()->pBindings = bindings_.data();
  return new DescriptorSetLayout(info_ptr());
}
}  // namespace prt::vk