#include "prette/sampler.h"

#include <string>

#include "prette/common.h"
#include "prette/device.h"
#include "prette/gfx.h"
#include "prette/to_string.h"
#include "prette/vk.h"

namespace prt::vk {
SamplerBuilder::SamplerBuilder() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  info_ptr()->magFilter = VK_FILTER_LINEAR;
  info_ptr()->minFilter = VK_FILTER_LINEAR;
  info_ptr()->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info_ptr()->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info_ptr()->addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  info_ptr()->anisotropyEnable = VK_TRUE;
  info_ptr()->maxAnisotropy = 16;
  info_ptr()->borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  info_ptr()->unnormalizedCoordinates = VK_FALSE;
  info_ptr()->compareEnable = VK_FALSE;
  info_ptr()->compareOp = VK_COMPARE_OP_ALWAYS;
  info_ptr()->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
}

SamplerBuilder::~SamplerBuilder() {
  // do nothing
}

auto SamplerBuilder::IsValid() const -> bool {
  return true;
}

auto SamplerBuilder::Build() -> Sampler* {
  ASSERT(IsValid());
  return new Sampler(info_ptr());
}

Sampler::Sampler(const VkSamplerCreateInfo* create_info) :
  HandleTemplate() {
  ASSERT(create_info);
  const auto driver = Driver::Get();
  driver->CreateSampler(create_info, handle_ptr());
}

Sampler::~Sampler() {
  const auto driver = Driver::Get();
  driver->DestroySampler(handle_ref());
}

auto Sampler::ToString() const -> std::string {
  return ToStringHelper<Sampler>{};
}
}  // namespace prt::vk