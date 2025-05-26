#include "prette/vk_physical_device.h"

#include <algorithm>
#include <glog/logging.h>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx_vk.h"
#include "prette/surface.h"
#include "prette/swapchain/swapchain_support.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_instance.h"  // IWYU pragma: keep

namespace prt::vk {
static const ExtensionSet kRequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_KHR_portability_subset",
};

auto GetRequiredPhysicalDeviceExtensions() -> ExtensionSet const& {
  return kRequiredExtensions;
}

auto GetAllDeviceExtensionProperties(VkPhysicalDevice device, std::vector<VkExtensionProperties>& properties) -> bool {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  properties.resize(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, &properties[0]);
  return properties.size() == count;
}

auto HasRequiredExtensions(const ExtensionSet& required, const char* layer) -> PhysicalDevicePredicate {
  static const PhysicalDevicePredicate kAlways = [](VkPhysicalDevice device) {
    return true;
  };
  if (required.empty())
    return kAlways;
  return [required, layer](VkPhysicalDevice device) {
    std::vector<VkExtensionProperties> available{};
    GetAllDeviceExtensionProperties(device, available);
    for (const auto& required_ext : required) {
      const auto extension = std::ranges::find_if(available, [required_ext](VkExtensionProperties properties) {
        return required_ext.compare(properties.extensionName) == 0;
      });
      if (extension == std::end(available))
        return false;
    }
    return true;
  };
}

auto PhysicalDevice::IsSupportedFormat(VkPhysicalDevice device, const VkImageTiling tiling,
                                       const VkFormatFeatureFlags features) -> FormatPredicate {
  return [device, tiling, features](const VkFormat& format) {
    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(device, format, &properties);
    const auto is_linear = tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features;
    const auto is_optimal =
        tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features;
    return is_linear || is_optimal;
  };
}

auto PhysicalDevice::FindSupportedFormat(const VkPhysicalDevice physical_device, const std::vector<VkFormat>& formats,
                                         FormatPredicate filter) -> std::optional<VkFormat> {
  const auto pos = std::ranges::find_if(formats, filter);
  return pos != std::end(formats) ? std::optional<VkFormat>{*pos} : std::nullopt;
}

void GetAllPhysicalDevices(VkInstance instance, std::vector<VkPhysicalDevice>& devices) {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  LOG_IF(FATAL, count == 0) << "failed to find GPUs w/ vulkan support.";
  devices.resize(count);
  vkEnumeratePhysicalDevices(instance, &count, &devices[0]);
}

PhysicalDeviceIterator::PhysicalDeviceIterator(VkInstance instance) {
  GetAllPhysicalDevices(instance, devices_);
  current_ = std::begin(devices_);
}

auto FindPhysicalDevice(VkInstance instance, PhysicalDevicePredicate filter) -> VkPhysicalDevice {
  std::vector<VkPhysicalDevice> devices{};
  GetAllPhysicalDevices(instance, devices);
  const auto pos = std::ranges::find_if(devices, filter);
  return pos != std::end(devices) ? (*pos) : VK_NULL_HANDLE;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle, const QueueFamilyIndices& queue_families) :
  HandleTemplate<VkPhysicalDevice>(handle) {
  ASSERT(IsInitialized());
  vkGetPhysicalDeviceProperties(handle_ref(), &properties_);
  vkGetPhysicalDeviceMemoryProperties(handle_ref(), &mem_properties_);
  const auto depth_format = FindSupportedFormat(handle_ref());
  LOG_IF(FATAL, !depth_format) << "failed to find supported depth format.";
  depth_format_ = (*depth_format);
  ASSERT(HasDepthFormat());
  queue_families_ = queue_families;
}

auto PhysicalDevice::FindMemoryType(const uint32_t filter, const VkMemoryPropertyFlags& flags) const
    -> std::optional<uint32_t> {
  for (auto idx = 0; idx < mem_properties_.memoryTypeCount; idx++) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    if ((filter & (1 << idx)) && ((mem_properties_.memoryTypes[idx].propertyFlags & flags) == flags))
      return {idx};
  }
  return std::nullopt;
}

auto PhysicalDevice::ToString() const -> std::string {
  return ToStringHelper<PhysicalDevice>{};
}

PhysicalDeviceResolver::PhysicalDeviceResolver() {
  required_extensions_.insert(std::begin(kRequiredExtensions), std::end(kRequiredExtensions));
}

auto PhysicalDeviceResolver::Resolve(const VkInstance instance, const VkSurfaceKHR surface) -> PhysicalDevice* {
  const PhysicalDevicePredicate filter = [surface, this](VkPhysicalDevice device) {
    const PhysicalDevicePredicate has_queue_families = [surface](VkPhysicalDevice device) {
      QueueFamilyIndices indices = FindQueueFamilies(device, surface);
      return indices.IsComplete();
    };
    if (!has_queue_families(device))
      return false;

    const PhysicalDevicePredicate extensions_supported = HasRequiredExtensions(required_extensions_);
    if (!extensions_supported(device))
      return false;

    const PhysicalDevicePredicate swap_supported = [surface](VkPhysicalDevice device) {
      return (bool)QuerySwapchainSupport(device, surface);
    };
    if (!swap_supported(device))
      return false;
    return has_queue_families(device) && extensions_supported(device) && swap_supported(device);
  };
  const auto physical_device = FindPhysicalDevice(instance, filter);
  if (physical_device == VK_NULL_HANDLE) {
    LOG(ERROR) << "failed to find suitable VkPhysicalDevice with the following extensions:";
    for (const auto& extension : required_extensions_) {
      LOG(ERROR) << " - " << extension;
    }
    LOG(FATAL) << "";
  }
#ifdef PRT_DEBUG
  if (VLOG_IS_ON(1)) {
    LOG(INFO) << "found suitable vulkan physical device:";
    PrintProperties(physical_device);
  }
#endif  // PRT_DEBUG
  return new PhysicalDevice(physical_device, FindQueueFamilies(physical_device, surface));
}

#ifdef PRT_DEBUG

void PrintPhysicalDevices(VkInstance instance) {
  LOG(INFO) << "Physical Devices: ";
  PhysicalDeviceIterator iter(instance);
  while (iter.HasNext()) {
    const auto next = iter.Next();
    PrintProperties(next);
  }
}

void PrintProperties(const VkPhysicalDeviceProperties& properties) {
  LOG(INFO) << "  * Device Name: " << properties.deviceName;
  LOG(INFO) << "  * Device Type: " << properties.deviceType;
  LOG(INFO) << "  * Vendor ID: " << properties.vendorID;
  LOG(INFO) << "  * Device ID: " << properties.deviceID;
  LOG(INFO) << "  * API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "."
            << VK_VERSION_MINOR(properties.apiVersion) << "." << VK_VERSION_PATCH(properties.apiVersion);
}

void PrintProperties(const VkPhysicalDevice device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  return PrintProperties(properties);
}

#endif  // PRT_DEBUG
}  // namespace prt::vk