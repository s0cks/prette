#include "prette/device_builder.h"

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"
#include "prette/gfx_vk.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"

namespace prt::vk {
static ExtensionSet kDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef OS_IS_OSX
    "VK_KHR_portability_subset",
#endif  // OS_IS_OSX
};

static LayerSet kDeviceLayers = {
#ifdef PRT_DEBUG
    "VK_LAYER_KHRONOS_validation",
#endif  // PRT_DEBUG
};

DeviceBuilder::DeviceBuilder() :
  vk::HandleBuilderTemplate<VkDeviceCreateInfo, Device>() {
  device_features_.samplerAnisotropy = VK_TRUE;
  device_features_.fillModeNonSolid = VK_TRUE;
}

DeviceBuilder::~DeviceBuilder() {}

auto DeviceBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement
  return true;
}

auto DeviceBuilder::WithQueues(const std::unordered_set<uint32_t>& rhs) -> DeviceBuilder& {
  queue_infos_.reserve(rhs.size());
  for (const auto& family : rhs) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &priority_;
    queue_infos_.push_back(queue_create_info);
  }
  return *this;
}

auto DeviceBuilder::WithQueues(const QueueFamilyIndices& rhs) -> DeviceBuilder& {
  const auto& queue_families = Driver::Get()->GetPhysicalDevice()->GetQueueFamilies();
  std::unordered_set<uint32_t> unique_families{};
  queue_families.GetUniqueFamilies(unique_families);
  return WithQueues(unique_families);
}

auto DeviceBuilder::Build() -> Device* {
  ASSERT(IsValid());
  info_ptr()->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  info_ptr()->pQueueCreateInfos = &queue_infos_[0];
  info_ptr()->queueCreateInfoCount = queue_infos_.size();
  info_ptr()->pEnabledFeatures = &device_features_;

  std::vector<const char*> extensions{};
  extensions.reserve(kDeviceExtensions.size());
  std::ranges::for_each(kDeviceExtensions, [&extensions](const std::string& ext) {
    extensions.push_back(ext.c_str());
  });
  info_ptr()->ppEnabledExtensionNames = extensions.data();
  info_ptr()->enabledExtensionCount = extensions.size();

  std::vector<const char*> layers{};
  layers.reserve(kDeviceLayers.size());
  std::ranges::for_each(kDeviceLayers, [&layers](const std::string& layer) {
    layers.push_back(layer.c_str());
  });
  info_ptr()->enabledLayerCount = layers.size();
  info_ptr()->ppEnabledLayerNames = layers.data();
  return new Device(info_ptr());
}
}  // namespace prt::vk