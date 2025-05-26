#ifndef PRT_DEVICE_BUILDER_H
#define PRT_DEVICE_BUILDER_H

#include <cstdint>
#include <unordered_set>
#include <vector>

#include "prette/gfx.h"
#include "prette/vk.h"

namespace prt::vk {
class DeviceBuilder : public vk::HandleBuilderTemplate<VkDeviceCreateInfo, Device> {
 private:
  float priority_ = kDefaultDevicePriority;
  VkPhysicalDeviceFeatures device_features_{};
  std::vector<VkDeviceQueueCreateInfo> queue_infos_{};

 public:
  DeviceBuilder();
  ~DeviceBuilder() override;

  auto WithQueues(const QueueFamilyIndices& rhs) -> DeviceBuilder&;
  auto WithQueues(const std::unordered_set<uint32_t>& rhs) -> DeviceBuilder&;

  auto WithPriority(const float rhs) -> DeviceBuilder& {
    priority_ = rhs;
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Device* override;
};
}  // namespace prt::vk

#endif  // PRT_DEVICE_BUILDER_H
