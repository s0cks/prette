#ifndef PRT_VK_INSTANCE_H
#define PRT_VK_INSTANCE_H

#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/vk.h"
#include "prette/vk_debug_messenger.h"

namespace prt::vk {
class Instance;
using InstancePtr = std::unique_ptr<Instance>;

class Instance : public HandleTemplate<VkInstance> {
 public:
  static constexpr const VkInstanceCreateFlags kDefaultFlags = 0;

 private:
#ifdef PRT_DEBUG
  DebugMessenger* debug_ = nullptr;
#endif  // PRT_DEBUG

 public:
  explicit Instance(const VkInstanceCreateInfo* create_info);
  ~Instance() override;

  auto ToString() const -> std::string override;
  operator VkInstance() const {
    return GetHandle();
  }
};

class InstanceBuilder : public vk::HandleBuilderTemplate<VkInstanceCreateInfo, Instance> {
 private:
  LayerSet required_layers_{};
  ExtensionSet required_extensions_{};
  VkInstanceCreateFlags flags_ = Instance::kDefaultFlags;

 public:
  explicit InstanceBuilder(const VkApplicationInfo* app_info);
  ~InstanceBuilder() override = default;

  auto WithLayers(Layer* layers, const uint64_t num_layers) -> InstanceBuilder& {
    ASSERT(layers && num_layers > 0);
    required_layers_.insert(layers, layers + num_layers);
    return *this;
  }

  auto WithLayers(const LayerSet& rhs) -> InstanceBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    required_layers_.insert(std::begin(rhs), std::end(rhs));
    return *this;
  }

  auto WithExtensions(Extension* extensions, const uint64_t num_extensions) -> InstanceBuilder& {
    ASSERT(extensions && num_extensions > 0);
    required_extensions_.insert(extensions, extensions + num_extensions);
    return *this;
  }

  auto WithExtensions(const ExtensionSet& rhs) -> InstanceBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    required_extensions_.insert(std::begin(rhs), std::end(rhs));
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> Instance* override;
};
}  // namespace prt::vk

#endif  // PRT_VK_INSTANCE_H
