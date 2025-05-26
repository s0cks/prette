#include "prette/descriptor_set_builder.h"

#include "prette/descriptor_set.h"

namespace prt::vk {
auto DescriptorSetBuilder::Build() -> DescriptorSet* {
  return DescriptorSet::New(name_, bindings_);
}
}  // namespace prt::vk