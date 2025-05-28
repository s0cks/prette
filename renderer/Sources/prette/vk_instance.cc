#include "prette/vk_instance.h"

#include <algorithm>
#include <glog/logging.h>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/gfx.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_debug_messenger.h"

namespace prt::vk {
Instance::Instance(const VkInstanceCreateInfo* create_info) {
  const auto driver = Driver::Get();
  driver->CreateInstance(create_info, handle_ptr());
#ifdef PRT_DEBUG
  // NOLINTNEXTLINE
  vk::DebugMessengerBuilder builder(this);
  // clang-format off
  builder.WithErrorSeverity()
    .WithWarningSeverity()
    .WithMaxMessageType()
    .WithCallback(DebugMessenger::OnDebugMessage);
  // clang-format on
  debug_ = builder.Build();
  ASSERT_INITIALIZED(debug_);
#endif  // PRT_DEBUG
}

Instance::~Instance() {
#ifdef PRT_DEBUG
  delete debug_;
#endif  // PRT_DEBUG
  const auto driver = Driver::Get();
  driver->DestroyInstance(handle_ref());
}

auto Instance::ToString() const -> std::string {
  return ToStringHelper<Instance>{};
}

InstanceBuilder::InstanceBuilder(const VkApplicationInfo* app_info) :
  vk::HandleBuilderTemplate<VkInstanceCreateInfo, Instance>() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  info_ptr()->pApplicationInfo = app_info;
  info_ptr()->flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;  // TODO: should this be default
}

auto InstanceBuilder::IsValid() const -> bool {
  return true;
}

auto InstanceBuilder::Build() -> Instance* {
  ASSERT(IsValid());
  // extensions
  std::vector<const char*> extensions{};
  extensions.reserve(required_extensions_.size());
  std::ranges::for_each(required_extensions_, [&extensions](const Extension& extension) {
    extensions.push_back(extension.c_str());
  });
  info_ptr()->enabledExtensionCount = extensions.size();
  info_ptr()->ppEnabledExtensionNames = &extensions[0];

  std::vector<const char*> layers{};
  layers.reserve(required_layers_.size());
  std::ranges::for_each(required_layers_, [&layers](const Layer& layer) {
    layers.push_back(layer.c_str());
  });
  info_ptr()->enabledLayerCount = layers.size();
  info_ptr()->ppEnabledLayerNames = layers.data();

#ifdef PRT_DEBUG
  vk::DebugMessengerBuilder builder{};
  // clang-format off
  builder.WithErrorSeverity()
    .WithWarningSeverity()
    .WithCallback(DebugMessenger::OnDebugMessage);
  // clang-format on
  info_ptr()->pNext = &builder.info();
#else
  info_ptr()->pNext = nullptr;
#endif  // PRT_DEBUG
  return new Instance(info_ptr());
}
}  // namespace prt::vk