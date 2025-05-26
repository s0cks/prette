#ifndef PRT_IMAGE_VIEW_H
#define PRT_IMAGE_VIEW_H

#include <string>

#include "prette/vk.h"

namespace prt::vk {
class ImageView : public HandleTemplate<VkImageView> {
  friend class ImageViewBuilder;

 private:
  ImageView(const VkImageViewCreateInfo* create_info);

 public:
  ~ImageView();
  auto ToString() const -> std::string override;

  operator VkImageView() const {
    return GetHandle();
  }
};

auto NewDepthImageView(vk::Image* image, const VkFormat format) -> vk::ImageView*;
}  // namespace prt::vk

#endif  // PRT_IMAGE_VIEW_H
