#ifndef PRT_SURFACE_H
#define PRT_SURFACE_H

#include <vulkan/vulkan_core.h>

#include "prette/platform.h"
#include "prette/vk.h"

namespace prt {
class Driver;
namespace vk {
class Instance;
class PhysicalDevice;
class Surface : public vk::HandleTemplate<VkSurfaceKHR> {
  friend class prt::Driver;

 private:
  VkSurfaceFormatKHR format_{};
  VkPresentModeKHR present_mode_{};
  VkExtent2D extent_{};
  uint32_t image_count_ = 0;
  VkSurfaceCapabilitiesKHR capabilities_{};

  Surface(Driver* driver);

 public:
  ~Surface();

  auto GetSurface() const -> const VkSurfaceKHR& {
    return GetHandle();
  }

  auto GetFormat() const -> const VkSurfaceFormatKHR& {
    return format_;
  }

  auto GetImageFormat() const -> const VkFormat& {
    return format_.format;
  }

  auto GetPresentMode() const -> const VkPresentModeKHR& {
    return present_mode_;
  }

  auto GetExtent() const -> const VkExtent2D& {
    return extent_;
  }

  auto GetCapabilities() const -> const VkSurfaceCapabilitiesKHR& {
    return capabilities_;
  }

  auto GetImageCount() const -> uint32_t {
    return image_count_;
  }

  auto ToString() const -> std::string override;
  operator VkSurfaceKHR() const {
    return GetHandle();
  }
};
}  // namespace vk
}  // namespace prt

#endif  // PRT_SURFACE_H
