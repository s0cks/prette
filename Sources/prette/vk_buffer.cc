#include "prette/vk_buffer.h"

#include <vector>

#include "prette/common.h"
#include "prette/copy_to_buffer.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/staging_scope.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"
#include "prette/vk_physical_device.h"

namespace prt::vk {
BufferBuilder::BufferBuilder() {
  info_ptr()->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info_ptr()->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

auto BufferBuilder::IsValid() const -> bool {
  return info().size > 0;
}

auto BufferBuilder::Build() -> Buffer* {
  ASSERT(IsValid());
  return new Buffer(info(), mem_flags_);
}

auto BufferBuilder::BuildStagingBuffer(const uint8_t* data, const uint64_t num_bytes,
                                       const VkMemoryPropertyFlags mem_flags) -> Buffer* {
  ASSERT(data && num_bytes > 0);
  const auto new_buffer = BuildTransferSourceBuffer(num_bytes, mem_flags);
  ASSERT_INITIALIZED(new_buffer);
  CopyBytesToBuffer copy(data, num_bytes);
  copy(new_buffer);
  return new_buffer;
}

Buffer::Buffer(const VkBufferCreateInfo& create_info, const VkMemoryPropertyFlags mem_flags) :
  size_(create_info.size) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  const auto& device = driver->GetDevice();
  driver->CreateBuffer(&create_info, &buffer_);
  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(*device, buffer_, &mem_requirements);
  AllocateMemory(driver, mem_requirements, memory_, mem_flags);
  CHECK_VK(FATAL, vkBindBufferMemory(*device, buffer_, memory_, 0), "failed to bind vk buffer memory");
  InitDescriptor(descriptor_, buffer_);
}

Buffer::Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) :
  size_(size) {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  const auto driver = Driver::Get();
  ASSERT(driver);

  const auto& device = driver->GetDevice();
  driver->CreateBuffer(&create_info, &buffer_);
  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(*device, buffer_, &mem_requirements);
  AllocateMemory(driver, mem_requirements, memory_, properties);
  CHECK_VK(FATAL, vkBindBufferMemory(*device, buffer_, memory_, 0), "failed to bind vk buffer memory");
  InitDescriptor(descriptor_, buffer_);
}

Buffer::~Buffer() {
  Destroy();
}

void Buffer::AllocateMemory(Driver* driver, const VkDeviceSize alloc_size, const uint32_t memory_type,
                            VkDeviceMemory& memory) {
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = alloc_size;
  alloc_info.memoryTypeIndex = memory_type;
  CHECK_VK(FATAL, vkAllocateMemory(*driver->GetDevice(), &alloc_info, driver->GetAllocator(), &memory),
           "failed to allocate vk memory");
  ASSERT(memory != VK_NULL_HANDLE);
}

void Buffer::AllocateMemory(Driver* driver, const VkMemoryRequirements& mem_requirements, VkDeviceMemory& memory,
                            VkMemoryPropertyFlags properties) {
  const auto mem_type = driver->GetPhysicalDevice()->FindMemoryType(mem_requirements.memoryTypeBits, properties);
  LOG_IF(FATAL, !mem_type) << "failed to find valid memory type.";
  return AllocateMemory(driver, mem_requirements.size, (*mem_type), memory);
}

void Buffer::AllocateImageMemory(Driver* driver, const VkImage& image, VkDeviceMemory& memory,
                                 VkMemoryPropertyFlags properties) {
  VkMemoryRequirements mem_requirements{};
  vkGetImageMemoryRequirements(*driver->GetDevice(), image, &mem_requirements);
  AllocateMemory(driver, mem_requirements, memory, properties);
}

void Buffer::CopyDataToImageWithStaging(const VkImage& image, const uint8_t* data, const uint64_t num_bytes,
                                        const std::vector<VkBufferImageCopy>& regions) {
  vk::StagingScope staging(data, num_bytes);
  SingleUseCommandBuffer buffer;
  vkCmdCopyBufferToImage(buffer, *staging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         static_cast<uint32_t>(regions.size()), regions.data());
}

void Buffer::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  if (memory_)
    vkFreeMemory(*driver->GetDevice(), memory_, driver->GetAllocator());
  driver->DestroyBuffer(buffer_);
}
}  // namespace prt::vk