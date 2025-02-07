#include <vulkan/vulkan_core.h>
#ifndef PRT_GFX_H
#error "Please #include <prt/gfx.h> instead."
#endif  // PRT_GFX_H

#ifndef PRT_GFX_VK_H
#define PRT_GFX_VK_H

#include <glog/logging.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <unordered_set>
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
  std::optional<uint32_t> present{};

  auto GetGraphicsFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto GetPresentFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto IsComplete() const -> bool {
    return graphics.has_value() && present.has_value();
  }

  void GetUniqueFamilies(std::unordered_set<uint32_t>& families) const {
    families.insert(present.value());
    families.insert(graphics.value());
  }

  void GetGraphicsQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetGraphicsFamily(), index, &queue);
  }

  void GetPresentQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetPresentFamily(), index, &queue);
  }
};

#define MAX_NUMBER_OF_FRAMES_IN_FLIGHT 2

static inline auto ClampExtent(VkExtent2D& v, const VkExtent2D& min, const VkExtent2D& max) -> VkExtent2D& {
  v.width = std::clamp(v.width, min.width, max.width);
  v.height = std::clamp(v.height, min.height, max.height);
  return v;
}

static inline auto ClampExtent(VkExtent2D& v, const VkSurfaceCapabilitiesKHR& surface_capabilities) -> VkExtent2D& {
  return ClampExtent(v, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent);
}

static inline auto FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> QueueFamilyIndices {
  QueueFamilyIndices indices{};
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &families[0]);
  auto idx = 0;
  for (const auto& family : families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = idx;
    }

    VkBool32 present_supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &present_supported);
    if (present_supported) {
      indices.present = idx;
    }

    if (indices.IsComplete()) {
      break;
    }

    idx++;
  }
  return indices;
}

static inline auto FindSuitablePhysicalDevice(const VkInstance& instance, VkPhysicalDevice* result,
                                              const std::function<bool(const VkPhysicalDevice&)>& predicate) -> bool {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  LOG_IF(FATAL, count == 0) << "failed to find GPUs w/ vulkan support.";
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, &devices[0]);
  const auto device = std::ranges::find_if(devices, predicate);
  if (device == std::end(devices)) {
    (*result) = VK_NULL_HANDLE;
    return false;
  }
  (*result) = (*device);
  ASSERT((*result) != VK_NULL_HANDLE);
  return true;
}

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
