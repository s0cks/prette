#include "prette/descriptor_set_update.h"

#include <utility>

#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
DescriptorSetUpdate::DescriptorSetUpdate(VkDescriptorSet dset, const WriteList writes) :
  dset_(dset),
  writes_(std::move(writes)) {}

DescriptorSetUpdate::~DescriptorSetUpdate() {
  if (!IsEmpty())
    Commit();
}

auto DescriptorSetUpdate::WriteBuilder::WithBufferInfo(Buffer* rhs) -> WriteBuilder& {
  ASSERT_INITIALIZED(rhs);
  return WithBufferInfo(&rhs->GetDescriptor());
}

void DescriptorSetUpdate::Commit() {
  ASSERT(!IsEmpty());
  const auto driver = Driver::Get();
  vkUpdateDescriptorSets(*driver->GetDevice(), GetNumberOfWrites(), writes(), 0, nullptr);
}
}  // namespace prt::vk