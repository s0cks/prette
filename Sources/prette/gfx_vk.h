#include <vulkan/vulkan_core.h>
#ifndef PRT_GFX_H
#error "Please #include <prt/gfx.h> instead."
#endif  // PRT_GFX_H

#ifndef PRT_GFX_VK_H
#define PRT_GFX_VK_H

#include <glog/logging.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "prette/common.h"

namespace prt {
struct MemoryBuffer {
  VkBuffer buffer{};
  VkDeviceMemory memory{};
};

static inline auto operator<<(std::ostream& stream, const VkResult& rhs) -> std::ostream& {
  return stream << std::string(string_VkResult(rhs));
}

#define CHECK_VK(Severity, Result, Message)                                \
  ({                                                                       \
    const auto result = (Result);                                          \
    LOG_IF(Severity, result != VK_SUCCESS) << (Message) << ": " << result; \
  })

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics{};

  auto IsComplete() const -> bool {
    return graphics.has_value();
  }
};

static inline auto FindQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices {
  QueueFamilyIndices indices{};
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &families[0]);
  const auto graphics_family = std::ranges::find_if(families, [](const VkQueueFamilyProperties& family) -> bool {
    return (family.queueFlags & VK_QUEUE_GRAPHICS_BIT);
  });
  if (graphics_family != std::end(families))
    indices.graphics = std::distance(std::begin(families), graphics_family);
  return indices;
}

static inline auto IsDeviceSuitable(VkPhysicalDevice device) -> bool {
  const auto indices = FindQueueFamilies(device);
  return indices.IsComplete();
}

static inline auto FindSuitablePhysicalDevice(const VkInstance& instance, VkPhysicalDevice* result) -> bool {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  LOG_IF(FATAL, count == 0) << "failed to find GPUs w/ vulkan support.";
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, &devices[0]);
  const auto device = std::ranges::find_if(devices, &IsDeviceSuitable);
  if (device == std::end(devices)) {
    (*result) = VK_NULL_HANDLE;
    return false;
  }
  (*result) = (*device);
  ASSERT((*result) != VK_NULL_HANDLE);
  return true;
}

struct SwapChain {
  VkSwapchainKHR chain{};
  std::vector<VkImage> images{};
  std::vector<VkImageView> views{};
  VkFormat format{};
  VkExtent2D extent{};
};

#ifdef PRT_DEBUG

static inline void PrintProperties(const VkPhysicalDevice& device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  LOG(INFO) << "  * Device Name: " << properties.deviceName;
  LOG(INFO) << "  * Device Type: " << properties.deviceType;
  LOG(INFO) << "  * Vendor ID: " << properties.vendorID;
  LOG(INFO) << "  * Device ID: " << properties.deviceID;
  LOG(INFO) << "  * API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "." << VK_VERSION_MINOR(properties.apiVersion)
            << "." << VK_VERSION_PATCH(properties.apiVersion);
}

#endif  // PRT_DEBUG
}  // namespace prt

#endif  // PRT_GFX_VK_H
