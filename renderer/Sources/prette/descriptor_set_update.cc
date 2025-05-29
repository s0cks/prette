#include "prette/descriptor_set_update.h"

#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/descriptor_set.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
DescriptorSetUpdate::DescriptorSetUpdate(VkDescriptorSet dset, const WriteList writes) :
  dset_(dset),
  writes_(std::move(writes)) {}

DescriptorSetUpdate::DescriptorSetUpdate(vk::DescriptorSet* dset) :
  DescriptorSetUpdate((*dset)) {}

DescriptorSetUpdate::~DescriptorSetUpdate() {
  if (!IsEmpty())
    Commit();
}

auto DescriptorSetUpdate::WriteBuilder::WithBufferInfo(Buffer* rhs) -> WriteBuilder& {
  ASSERT_INITIALIZED(rhs);
  return WithBufferInfo(&rhs->GetDescriptor());
}

auto DescriptorSetUpdate::WriteBuilder::WithCameraBuffer(Camera* rhs) -> WriteBuilder& {
  ASSERT(rhs);
  return WithBufferInfo(rhs->GetBuffer());
}

void DescriptorSetUpdate::Commit() {
  ASSERT(!IsEmpty());
  const auto driver = Driver::Get();
  vkUpdateDescriptorSets(*driver->GetDevice(), GetNumberOfWrites(), writes(), 0, nullptr);
}
}  // namespace prt::vk