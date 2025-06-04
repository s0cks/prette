#ifndef PRT_IMAGE_VIEW_BUILDER_H
#define PRT_IMAGE_VIEW_BUILDER_H

#include <vector>

#include "prette/vk.h"

namespace prt::vk {
class ImageViewBuilder : public HandleBuilderTemplate<VkImageViewCreateInfo, ImageView> {
 public:
  ImageViewBuilder();
  explicit ImageViewBuilder(Image* image);
  ~ImageViewBuilder() override = default;

  auto WithFormat(const VkFormat rhs) -> ImageViewBuilder& {
    info_ptr()->format = rhs;
    return *this;
  }

  auto WithType(const VkImageViewType rhs) -> ImageViewBuilder& {
    info_ptr()->viewType = rhs;
    return *this;
  }

  auto WithImage(const VkImage rhs) -> ImageViewBuilder& {
    info_ptr()->image = rhs;
    return *this;
  }

  auto WithImage(const Image* rhs) -> ImageViewBuilder&;

  auto WithSubresourceRange(const VkImageSubresourceRange rhs) -> ImageViewBuilder& {
    info_ptr()->subresourceRange = rhs;
    return *this;
  }

  auto WithComponents(const VkComponentMapping rhs) -> ImageViewBuilder& {
    info_ptr()->components = rhs;
    return *this;
  }

  auto WithComponents(const VkComponentSwizzle r, const VkComponentSwizzle g, const VkComponentSwizzle b,
                      const VkComponentSwizzle a) -> ImageViewBuilder& {
    info_ptr()->components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    info_ptr()->components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    info_ptr()->components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    info_ptr()->components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    return *this;
  }

  inline auto WithComponents(const VkComponentSwizzle rhs) -> ImageViewBuilder& {
    return WithComponents(rhs, rhs, rhs, rhs);
  }

  inline auto WithIdentityComponents() -> ImageViewBuilder& {
    return WithComponents(VK_COMPONENT_SWIZZLE_IDENTITY);
  }

  auto GetAspectMask() const -> VkImageAspectFlags {
    return info().subresourceRange.aspectMask;
  }

  auto WithAspectMask(const VkImageAspectFlags rhs) -> ImageViewBuilder& {
    info_ptr()->subresourceRange.aspectMask = rhs;
    return *this;
  }

  inline auto WithColorAspectMask() -> ImageViewBuilder& {
    return WithAspectMask(VK_IMAGE_ASPECT_COLOR_BIT);
  }

  inline auto WithDepthAspectMask() -> ImageViewBuilder& {
    return WithAspectMask(VK_IMAGE_ASPECT_DEPTH_BIT);
  }

  auto IsValid() const -> bool override;
  auto Build() -> ImageView* override;
  void BuildAll(const std::vector<VkImage>& images, std::vector<ImageView*>& results);
  void BuildAll(const std::vector<Image*>& images, std::vector<ImageView*>& results);

  auto operator()() -> ImageView* {
    return Build();
  }

  operator ImageView*() {
    return Build();
  }
};
}  // namespace prt::vk

#endif  // PRT_IMAGE_VIEW_BUILDER_H
