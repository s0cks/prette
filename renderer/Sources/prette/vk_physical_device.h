#ifndef PRT_VK_PHYSICAL_DEVICE_H
#define PRT_VK_PHYSICAL_DEVICE_H

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx_vk.h"
#include "prette/vk.h"

namespace prt {
class Driver;

namespace vk {
using PhysicalDevicePredicate = std::function<bool(const VkPhysicalDevice&)>;

auto GetAllDeviceExtensionProperties(VkPhysicalDevice device, std::vector<VkExtensionProperties>& properties) -> bool;
auto GetRequiredPhysicalDeviceExtensions() -> ExtensionSet const&;
auto HasRequiredExtensions(const ExtensionSet& required, const char* layer = nullptr) -> PhysicalDevicePredicate;
void GetAllPhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& devices);
auto FindPhysicalDevice(VkInstance instance, PhysicalDevicePredicate filter) -> VkPhysicalDevice;

class PhysicalDevice : public HandleTemplate<VkPhysicalDevice> {
  friend class PhysicalDeviceResolver;
  DEFINE_DEFAULT_COPYABLE_TYPE(PhysicalDevice);

 public:
  static constexpr const auto kDefaultDepthFormats = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                                      VK_FORMAT_D24_UNORM_S8_UINT};
  static constexpr const auto kDefaultDepthFormatTiling = VK_IMAGE_TILING_OPTIMAL;
  static constexpr const auto kDefaultDepthFormatFeatures = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

  static auto IsSupportedFormat(VkPhysicalDevice physical_device,
                                const VkImageTiling tiling = kDefaultDepthFormatTiling,
                                const VkFormatFeatureFlags features = kDefaultDepthFormatFeatures) -> FormatPredicate;
  static auto FindSupportedFormat(VkPhysicalDevice physical_device, const std::vector<VkFormat>& format,
                                  FormatPredicate filter) -> std::optional<VkFormat>;

  static inline auto FindSupportedFormat(VkPhysicalDevice physical_device) -> std::optional<VkFormat> {
    static const auto filter = IsSupportedFormat(physical_device);
    return FindSupportedFormat(physical_device, kDefaultDepthFormats, filter);
  }

 private:
  QueueFamilyIndices queue_families_{};
  VkFormat depth_format_ = VK_FORMAT_UNDEFINED;
  VkPhysicalDeviceProperties properties_{};
  VkPhysicalDeviceMemoryProperties mem_properties_{};

  explicit PhysicalDevice(VkPhysicalDevice handle, const QueueFamilyIndices& queue_families);

 public:
  ~PhysicalDevice() override = default;

  auto GetQueueFamilies() const -> const QueueFamilyIndices& {
    return queue_families_;
  }

  auto GetProperties() const -> const VkPhysicalDeviceProperties& {
    return properties_;
  }

  auto GetDepthFormat() const -> VkFormat {
    return depth_format_;
  }

  inline auto HasDepthFormat() const -> bool {
    return GetDepthFormat() != VK_FORMAT_UNDEFINED;
  }

  auto FindMemoryType(const uint32_t filter, const VkMemoryPropertyFlags& flags) const -> std::optional<uint32_t>;

  auto ToString() const -> std::string override;
  operator VkPhysicalDevice() const {
    return GetHandle();
  }
};

class PhysicalDeviceIterator {
  using DeviceList = std::vector<VkPhysicalDevice>;

 private:
  DeviceList devices_{};
  DeviceList::iterator current_{};

 public:
  explicit PhysicalDeviceIterator(VkInstance instance);
  ~PhysicalDeviceIterator() = default;

  auto HasNext() const -> bool {
    return current_ != std::end(devices_);
  }

  auto Next() -> VkPhysicalDevice {
    const auto next = (*current_);
    current_++;
    return next;
  }
};

class PhysicalDeviceResolver {
 private:
  ExtensionSet required_extensions_{};
  PhysicalDevicePredicate filter_ = std::identity();

 public:
  PhysicalDeviceResolver();
  ~PhysicalDeviceResolver() = default;

  auto WithRequiredExtensions(const Extension* extensions, const uint64_t num_extensions) -> PhysicalDeviceResolver& {
    ASSERT(extensions && num_extensions > 0);
    required_extensions_.insert(extensions, extensions + num_extensions);
    return *this;
  }

  auto WithRequiredExtensions(const ExtensionSet& rhs) -> PhysicalDeviceResolver& {
    ASSERT(!rhs.empty());
    required_extensions_.insert(std::begin(rhs), std::end(rhs));
    return *this;
  }

  auto WithRequiredExtensions(const ExtensionList& rhs) -> PhysicalDeviceResolver& {
    ASSERT(!rhs.empty());
    return WithRequiredExtensions(rhs.data(), rhs.size());
  }

  auto Resolve(const VkInstance instance, const VkSurfaceKHR surface) -> PhysicalDevice*;
};

#ifdef PRT_DEBUG

void PrintPhysicalDevices(VkInstance instance);

void PrintProperties(const VkPhysicalDeviceProperties& properties);
void PrintProperties(const VkPhysicalDevice device);

#endif  // PRT_DEBUG
}  // namespace vk
}  // namespace prt

#endif  // PRT_VK_PHYSICAL_DEVICE_H
