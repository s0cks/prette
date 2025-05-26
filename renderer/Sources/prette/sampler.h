#ifndef PRT_SAMPLER_H
#define PRT_SAMPLER_H

#include <string>

#include "prette/vk.h"

namespace prt::vk {
class Sampler;
class SamplerBuilder : public NamedHandleBuilderTemplate<VkSamplerCreateInfo, Sampler, SamplerBuilder> {
 public:
  SamplerBuilder();
  ~SamplerBuilder();

  auto WithMinFilter(const VkFilter rhs) -> SamplerBuilder& {
    info_ptr()->minFilter = rhs;
    return *this;
  }

  auto WithMagFilter(const VkFilter rhs) -> SamplerBuilder& {
    info_ptr()->magFilter = rhs;
    return *this;
  }

  auto WithFilter(const VkFilter rhs) -> SamplerBuilder& {
    info_ptr()->minFilter = info_ptr()->magFilter = rhs;
    return *this;
  }

  inline auto WithLinearFilter() -> SamplerBuilder& {
    return WithFilter(VK_FILTER_LINEAR);
  }

  inline auto WithNearestFilter() -> SamplerBuilder& {
    return WithFilter(VK_FILTER_NEAREST);
  }

  auto WithAddressModeU(const VkSamplerAddressMode rhs) -> SamplerBuilder& {
    info_ptr()->addressModeU = rhs;
    return *this;
  }

  auto WithAddressModeV(const VkSamplerAddressMode rhs) -> SamplerBuilder& {
    info_ptr()->addressModeV = rhs;
    return *this;
  }

  auto WithAddressModeW(const VkSamplerAddressMode rhs) -> SamplerBuilder& {
    info_ptr()->addressModeW = rhs;
    return *this;
  }

  auto WithAddressMode(const VkSamplerAddressMode u, const VkSamplerAddressMode v, const VkSamplerAddressMode w)
      -> SamplerBuilder& {
    info_ptr()->addressModeU = u;
    info_ptr()->addressModeV = v;
    info_ptr()->addressModeW = w;
    return *this;
  }

  inline auto WithAddressMode(const VkSamplerAddressMode rhs) -> SamplerBuilder& {
    return WithAddressMode(rhs, rhs, rhs);
  }

  inline auto WithRepeatAddressMode() -> SamplerBuilder& {
    return WithAddressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT);
  }

  inline auto WithClampToEdgeAddressMode() -> SamplerBuilder& {
    return WithAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
  }

  auto WithAnisotrophy(const bool rhs) -> SamplerBuilder& {
    info_ptr()->anisotropyEnable = rhs;
    return *this;
  }

  inline auto WithAnisotrophyEnabled() -> SamplerBuilder& {
    return WithAnisotrophy(true);
  }

  inline auto WithAnisotrophyDisabled() -> SamplerBuilder& {
    return WithAnisotrophy(false);
  }

  auto WithMaxAnisotrophy(const float rhs) -> SamplerBuilder& {
    info_ptr()->maxAnisotropy = rhs;
    return *this;
  }

  auto WithBorderColor(const VkBorderColor rhs) -> SamplerBuilder& {
    info_ptr()->borderColor = rhs;
    return *this;
  }

  auto WithCompare(const bool rhs) -> SamplerBuilder& {
    info_ptr()->compareEnable = rhs;
    return *this;
  }

  inline auto WithCompareEnabled() -> SamplerBuilder& {
    return WithCompare(true);
  }

  inline auto WithCompareDisabled() -> SamplerBuilder& {
    return WithCompare(false);
  }

  auto WithCompareOp(const VkCompareOp rhs) -> SamplerBuilder& {
    info_ptr()->compareOp = rhs;
    return *this;
  }

  auto WithMipmapMode(const VkSamplerMipmapMode rhs) -> SamplerBuilder& {
    info_ptr()->mipmapMode = rhs;
    return *this;
  }

  inline auto WithNearestMipmapMode() -> SamplerBuilder& {
    return WithMipmapMode(VK_SAMPLER_MIPMAP_MODE_NEAREST);
  }

  inline auto WithLinearMipmapMode() -> SamplerBuilder& {
    return WithMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR);
  }

  auto WithUnnormalizedCoords(const bool rhs) -> SamplerBuilder& {
    info_ptr()->unnormalizedCoordinates = rhs;
    return *this;
  }

  inline auto WithUnnormalizedCoordsEnabled() -> SamplerBuilder& {
    return WithUnnormalizedCoords(true);
  }

  inline auto WithUnnormalizedCoordsDisabled() -> SamplerBuilder& {
    return WithUnnormalizedCoords(false);
  }

  auto WithMinLod(const float rhs) -> SamplerBuilder& {
    info_ptr()->minLod = rhs;
    return *this;
  }

  auto WithMaxLod(const float rhs) -> SamplerBuilder& {
    info_ptr()->maxLod = rhs;
    return *this;
  }

  auto WithMipBias(const float rhs) -> SamplerBuilder& {
    info_ptr()->mipLodBias = rhs;
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Sampler* override;

  auto operator()() -> Sampler* {
    return Build();
  }

  operator Sampler*() {
    return Build();
  }
};

class Sampler : public HandleTemplate<VkSampler> {
  using Handle = VkSampler;
  friend class SamplerBuilder;

 private:
  Sampler(const VkSamplerCreateInfo* create_info);

 public:
  virtual ~Sampler();

  auto ToString() const -> std::string override;

  operator VkSampler() const {
    return GetHandle();
  }
};
}  // namespace prt::vk

#endif  // PRT_SAMPLER_H
