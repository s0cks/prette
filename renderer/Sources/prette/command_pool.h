#ifndef PRT_COMMAND_POOL_H
#define PRT_COMMAND_POOL_H

#include <string>
#include <vulkan/vulkan_core.h>

#include "prette/platform.h"
#include "prette/vk.h"

namespace prt::vk {
class CommandPool;
class CommandPoolBuilder : public vk::HandleBuilderTemplate<VkCommandPoolCreateInfo, CommandPool> {
 public:
  CommandPoolBuilder();
  ~CommandPoolBuilder() override = default;

  auto WithNext(const void* rhs) -> CommandPoolBuilder& {
    info_ptr()->pNext = rhs;
    return *this;
  }

  auto WithFlags(const VkCommandPoolCreateFlags rhs) -> CommandPoolBuilder& {
    info_ptr()->flags = rhs;
    return *this;
  }

  inline auto WithResetCommandBufferFlag() -> CommandPoolBuilder& {
    return WithFlags(info_ptr()->flags | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  }

  auto WithQueueFamilyIndex(const uint32_t rhs) -> CommandPoolBuilder& {
    info_ptr()->queueFamilyIndex = rhs;
    return *this;
  }

  auto IsValid() const -> bool override;
  auto Build() -> CommandPool* override;
};

class CommandPool : public vk::HandleTemplate<VkCommandPool> {
  friend class CommandPoolBuilder;

 private:
  explicit CommandPool(const VkCommandPoolCreateInfo* create_info);

 public:
  ~CommandPool() override;

  void AllocCommandBuffers(const VkCommandBufferAllocateInfo* alloc_info, VkCommandBuffer* results);
  void FreeCommandBuffers(VkCommandBuffer* buffers, const uint64_t num_buffers);
  auto ToString() const -> std::string override;

  operator VkCommandPool() const {
    return GetHandle();
  }
};
}  // namespace prt::vk

#endif  // PRT_COMMAND_POOL_H
