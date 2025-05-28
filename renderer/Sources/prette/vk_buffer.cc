#include "prette/vk_buffer.h"

#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/alloc_mem_req.h"
#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/vk.h"
#include "prette/vk_physical_device.h"

namespace prt::vk {
auto BaseBufferBuilder::IsValid() const -> bool {
  return info().size > 0;
}

auto BaseBufferBuilder::Build(VkMemoryPropertyFlags mem_flags) -> Buffer* {
  ASSERT(IsValid());
  return new Buffer(info(), mem_flags);
}

Buffer::Buffer(const VkBufferCreateInfo& create_info, const VkDeviceMemory memory) {
  if (create_info.size > 0) {
    const auto driver = Driver::Get();
    driver->CreateBuffer(&create_info, &buffer_);
    SetMemory(memory);
    size_ = create_info.size;
    InitDescriptor(descriptor_, buffer_, size_);
  }
}

Buffer::Buffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
  size_(create_info.size) {
  if (create_info.size > 0) {
    const auto driver = Driver::Get();
    ASSERT(driver);
    const auto& device = driver->GetDevice();
    driver->CreateBuffer(&create_info, &buffer_);
    AllocMemoryRequest alloc_mem(buffer_, mem_flags);
    InitMemory(alloc_mem);
    InitDescriptor(descriptor_, buffer_);
  }
}

static inline auto NewBufferCreateInfo(const VkDeviceSize size, const VkBufferUsageFlags usage) -> VkBufferCreateInfo {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = kDefaultBufferSharingMode;
  return std::move(create_info);
}

Buffer::Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags mem_flags) :
  Buffer(NewBufferCreateInfo(size, usage), mem_flags) {}

Buffer::~Buffer() {
  Destroy();
}

void Buffer::InitMemory(AllocMemoryRequest& alloc_mem) {
  alloc_mem(&memory_);
  BindMemory();
}

void Buffer::BindMemory() {
  const auto driver = Driver::Get();
  const Result status = vkBindBufferMemory(*driver->GetDevice(), buffer_, memory_, 0);
  LOG_IF(FATAL, !status) << "failed to bind VkBuffer memory: " << status;
}

void Buffer::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  if (memory_)
    vkFreeMemory(*driver->GetDevice(), memory_, driver->GetAllocator());
  driver->DestroyBuffer(buffer_);
}
}  // namespace prt::vk