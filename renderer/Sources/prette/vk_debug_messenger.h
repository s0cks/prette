#ifndef PRT_VK_DEBUG_MESSENGER_H
#define PRT_VK_DEBUG_MESSENGER_H

#ifdef PRT_DEBUG

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/vk.h"

namespace prt {
class Driver;
}

namespace prt::vk {
class DebugMessenger;
class DebugMessengerBuilder : public HandleBuilderTemplate<VkDebugUtilsMessengerCreateInfoEXT, DebugMessenger> {
 private:
  vk::Instance* instance_ = nullptr;

 public:
  explicit DebugMessengerBuilder(vk::Instance* instance = nullptr);
  ~DebugMessengerBuilder() override = default;

  auto GetSeverity() const -> VkDebugUtilsMessageSeverityFlagsEXT {
    return info().messageSeverity;
  }

  auto WithSeverity(const VkDebugUtilsMessageSeverityFlagsEXT rhs) -> DebugMessengerBuilder& {
    info_ptr()->messageSeverity = rhs;
    return *this;
  }

  inline auto WithVerboseSeverity() -> DebugMessengerBuilder& {
    return WithSeverity(GetSeverity() | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT);
  }

  inline auto WithWarningSeverity() -> DebugMessengerBuilder& {
    return WithSeverity(GetSeverity() | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT);
  }

  inline auto WithErrorSeverity() -> DebugMessengerBuilder& {
    return WithSeverity(GetSeverity() | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
  }

  inline auto WithInfoSeverity() -> DebugMessengerBuilder& {
    return WithSeverity(GetSeverity() | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT);
  }

  inline auto WithMaxSeverity() -> DebugMessengerBuilder& {
    return WithSeverity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT);
  }

  auto GetMessageType() const -> VkDebugUtilsMessageTypeFlagsEXT {
    return info().messageType;
  }

  auto WithMessageType(const VkDebugUtilsMessageTypeFlagsEXT rhs) -> DebugMessengerBuilder& {
    info_ptr()->messageType = rhs;
    return *this;
  }

  inline auto WithGeneralMessageType() -> DebugMessengerBuilder& {
    return WithMessageType(GetMessageType() | VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
  }

  inline auto WithValidationMessageType() -> DebugMessengerBuilder& {
    return WithMessageType(GetMessageType() | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
  }

  inline auto WithPerformanceMessageType() -> DebugMessengerBuilder& {
    return WithMessageType(GetMessageType() | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
  }

  inline auto WithDeviceAddressBindingMessageType() -> DebugMessengerBuilder& {
    return WithMessageType(GetMessageType() | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT);
  }

  inline auto WithMaxMessageType() -> DebugMessengerBuilder& {
    return WithMessageType(VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT);
  }

  auto WithCallback(PFN_vkDebugUtilsMessengerCallbackEXT rhs) -> DebugMessengerBuilder& {
    info_ptr()->pfnUserCallback = rhs;
    return *this;
  }

  auto GetInstance() const -> vk::Instance* {
    return instance_;
  }

  inline auto HasInstance() const -> bool {
    return GetInstance() != nullptr;
  }

  auto WithInstance(vk::Instance* rhs) -> DebugMessengerBuilder&;

  auto IsValid() const -> bool override;
  auto Build() -> DebugMessenger* override;
};

class DebugMessenger : public HandleTemplate<VkDebugUtilsMessengerEXT> {
 public:
  static VKAPI_ATTR auto VKAPI_CALL OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                   VkDebugUtilsMessageTypeFlagsEXT type,
                                                   const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                   void* userData) -> VkBool32;

 public:
  DebugMessenger() = default;
  explicit DebugMessenger(vk::Instance* instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info);
  ~DebugMessenger();

  auto ToString() const -> std::string override;
  operator VkDebugUtilsMessengerEXT() const {
    return GetHandle();
  }
};

}  // namespace prt::vk

#endif  // PRT_DEBUG
#endif  // PRT_VK_DEBUG_MESSENGER_H
