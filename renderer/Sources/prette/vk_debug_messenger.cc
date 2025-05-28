#include "prette/vk_debug_messenger.h"
#ifdef PRT_DEBUG

#include <glog/logging.h>
#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_instance.h"  // IWYU pragma: keep

namespace prt::vk {
VKAPI_ATTR auto VKAPI_CALL DebugMessenger::OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                          VkDebugUtilsMessageTypeFlagsEXT type,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                          void* userData) -> VkBool32 {
  switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      LOG(ERROR) << "vk validation layer: " << callbackData->pMessage;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      LOG(WARNING) << "vk validation layer: " << callbackData->pMessage;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      LOG(INFO) << "vk validation layer: " << callbackData->pMessage;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      VLOG(1) << "vk validation layer: " << callbackData->pMessage;
      break;
    default:
      LOG(WARNING) << "vk validation layer: " << callbackData->pMessage;
  }
  return VK_FALSE;
}

DebugMessengerBuilder::DebugMessengerBuilder(vk::Instance* instance) :
  prt::vk::HandleBuilderTemplate<VkDebugUtilsMessengerCreateInfoEXT, DebugMessenger>(),
  instance_(instance) {
  info_ptr()->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info_ptr()->messageSeverity = 0;
  info_ptr()->messageType = 0;
  info_ptr()->pfnUserCallback = nullptr;
  info_ptr()->pNext = nullptr;
}

auto DebugMessengerBuilder::WithInstance(vk::Instance* rhs) -> DebugMessengerBuilder& {
  ASSERT_INITIALIZED(rhs);
  instance_ = rhs;
  return *this;
}

auto DebugMessengerBuilder::IsValid() const -> bool {
  NOT_IMPLEMENTED(ERROR);  // TODO: implement
  return HasInstance();
}

auto DebugMessengerBuilder::Build() -> DebugMessenger* {
  ASSERT(IsValid());
  return new DebugMessenger(GetInstance(), info_ptr());
}

DebugMessenger::DebugMessenger(vk::Instance* instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info) :
  DebugMessenger() {
  const auto driver = Driver::Get();
  driver->CreateDebugUtilsMessenger(*instance, create_info, handle_ptr());
}

DebugMessenger::~DebugMessenger() {
  const auto driver = Driver::Get();
  driver->DestroyDebugUtilsMessenger(handle_ref());
}

auto DebugMessenger::ToString() const -> std::string {
  return ToStringHelper<DebugMessenger>{};
}
}  // namespace prt::vk

#endif  // PRT_DEBUG