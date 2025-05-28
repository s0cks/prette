#include "prette/mapped_buffer_scope.h"

#include <cstring>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/device.h"  // IWYU pragma: keep
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/vk.h"
#include "prette/vk_buffer.h"

namespace prt::vk {
void MappedBufferScope::Flush(const VkDeviceSize num_bytes, const VkDeviceSize offset) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  VkMappedMemoryRange range{};
  range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.memory = buffer_->GetMemory();
  range.offset = offset;
  range.size = num_bytes;
  CHECK_VK(FATAL, vkFlushMappedMemoryRanges(*driver->GetDevice(), 1, &range), "failed to flush mapped vk memory range");
}

MappedBufferScope::MappedBufferScope(Buffer* buffer, const VkDeviceSize size, const VkDeviceSize offset,
                                     const VkMemoryMapFlags flags) :
  buffer_(buffer) {
  ASSERT(buffer_);
  const auto status =
      GetDriverDevice()->MapDeviceMemory(GetBuffer()->GetMemory(), size, &mapped_memory_, offset, flags);
  LOG_IF(FATAL, !status) << "failed to map buffer memory: " << status;
}

MappedBufferScope::~MappedBufferScope() {
  if (!IsMapped())
    return;
  GetDriverDevice()->UnmapDeviceMemory(GetBuffer()->GetMemory());
}
}  // namespace prt::vk