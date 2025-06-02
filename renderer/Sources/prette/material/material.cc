#include "prette/material/material.h"

#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/descriptor_set_update.h"
#include "prette/material/material_system.h"

namespace prt {
Material::Material(const std::string name, const MaterialTextureArray textures) :
  name_(std::move(name)),
  textures_(std::move(textures)) {
  // NOLINTNEXTLINE(cppcoreguidelines-prefer-member-initializer)
  descriptors_ = MaterialSystem::GetSystem()->NewMaterialDescriptorSet(name_);
  ASSERT_INITIALIZED(descriptors_);
  UpdateDescriptorSet();
}

void Material::UpdateDescriptorSet() {
  vk::DescriptorSetUpdate update(GetDescriptorSet());
  // clang-format off
  update.AddWriteCombinedImageSampler(0)
    .WithImageInfo(GetAlbedoTexture());
  update.AddWriteCombinedImageSampler(1)
    .WithImageInfo(GetNormalTexture());
  update.AddWriteCombinedImageSampler(2)
    .WithImageInfo(GetMetallicTexture());
  update.AddWriteCombinedImageSampler(3)
    .WithImageInfo(GetRoughnessTexture());
  update.AddWriteCombinedImageSampler(4)
    .WithImageInfo(GetAoTexture());
  // clang-format on
}
}  // namespace prt