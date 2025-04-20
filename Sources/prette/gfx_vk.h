#ifndef PRT_GFX_VK_H
#define PRT_GFX_VK_H

#include <glog/logging.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <unordered_set>
#include <vector>

#include "prette/common.h"
#include "prette/gfx_driver.h"
#include "prette/lua.h"
#include "prette/prette.h"

#ifndef VK_DEFAULT_FENCE_TIMEOUT
#define VK_DEFAULT_FENCE_TIMEOUT 100000000000
#endif  // VK_DEFAULT_FENCE_TIMEOUT

#ifndef VK_FLAGS_NONE
#define VK_FLAGS_NONE 0
#endif  // VK_FLAGS_NONE

namespace prt {
class Driver;

static inline auto operator==(const VkExtent2D& lhs, const VkExtent2D& rhs) -> bool {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

static inline auto operator!=(const VkExtent2D& lhs, const VkExtent2D& rhs) -> bool {
  return lhs.width != rhs.width || lhs.height != rhs.height;
}

struct MemoryBuffer {
  VkBuffer buffer{};
  VkDeviceMemory memory{};
};

static inline auto operator<<(std::ostream& stream, const VkResult& rhs) -> std::ostream& {
  return stream << std::string(string_VkResult(rhs));
}

#define CHECK_VK(Severity, Result, Message)                                \
  ({                                                                       \
    const auto result = (Result);                                          \
    LOG_IF(Severity, result != VK_SUCCESS) << (Message) << ": " << result; \
  })

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics{};
  std::optional<uint32_t> present{};

  auto GetGraphicsFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto GetPresentFamily() const -> uint32_t {
    ASSERT(graphics.has_value());
    return graphics.value();
  }

  auto IsComplete() const -> bool {
    return graphics.has_value() && present.has_value();
  }

  void GetUniqueFamilies(std::unordered_set<uint32_t>& families) const {
    families.insert(present.value());
    families.insert(graphics.value());
  }

  void GetGraphicsQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetGraphicsFamily(), index, &queue);
  }

  void GetPresentQueue(const VkDevice& device, VkQueue& queue, const int index = 0) const {
    vkGetDeviceQueue(device, GetPresentFamily(), index, &queue);
  }
};

#define MAX_NUMBER_OF_FRAMES_IN_FLIGHT 2

#define VK_MAKE_PRT_VERSION            VK_MAKE_VERSION(PRT_VERSION_MAJOR, PRT_VERSION_MINOR, PRT_VERSION_PATCH)

static inline auto ClampExtent(VkExtent2D& v, const VkExtent2D& min, const VkExtent2D& max) -> VkExtent2D& {
  v.width = std::clamp(v.width, min.width, max.width);
  v.height = std::clamp(v.height, min.height, max.height);
  return v;
}

static inline auto ClampExtent(VkExtent2D& v, const VkSurfaceCapabilitiesKHR& surface_capabilities) -> VkExtent2D& {
  return ClampExtent(v, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent);
}

static inline auto FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> QueueFamilyIndices {
  QueueFamilyIndices indices{};
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> families(count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &count, &families[0]);
  auto idx = 0;
  for (const auto& family : families) {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphics = idx;
    }

    VkBool32 present_supported = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, surface, &present_supported);
    if (present_supported) {
      indices.present = idx;
    }

    if (indices.IsComplete()) {
      break;
    }

    idx++;
  }
  return indices;
}

static inline auto FindSuitablePhysicalDevice(const VkInstance& instance, VkPhysicalDevice* result,
                                              const std::function<bool(const VkPhysicalDevice&)>& predicate) -> bool {
  uint32_t count = 0;
  vkEnumeratePhysicalDevices(instance, &count, nullptr);
  LOG_IF(FATAL, count == 0) << "failed to find GPUs w/ vulkan support.";
  std::vector<VkPhysicalDevice> devices(count);
  vkEnumeratePhysicalDevices(instance, &count, &devices[0]);
  const auto device = std::ranges::find_if(devices, predicate);
  if (device == std::end(devices)) {
    (*result) = VK_NULL_HANDLE;
    return false;
  }
  (*result) = (*device);
  ASSERT((*result) != VK_NULL_HANDLE);
  return true;
}

static inline auto FindSupportedFormat(const VkPhysicalDevice physical_device, const std::vector<VkFormat>& formats,
                                       VkImageTiling tiling, VkFormatFeatureFlags features) -> VkFormat {
  for (const auto& format : formats) {
    VkFormatProperties properties{};
    vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);
    if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  LOG(FATAL) << "failed to find supported depth format";
}

static inline auto FindDepthFormat(const VkPhysicalDevice physical_device) -> VkFormat {
  return FindSupportedFormat(physical_device, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                             VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

static inline auto HasStencilComponent(const VkFormat format) -> bool {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

#ifdef PRT_DEBUG

static inline void PrintProperties(const VkPhysicalDevice& device) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(device, &properties);
  LOG(INFO) << "  * Device Name: " << properties.deviceName;
  LOG(INFO) << "  * Device Type: " << properties.deviceType;
  LOG(INFO) << "  * Vendor ID: " << properties.vendorID;
  LOG(INFO) << "  * Device ID: " << properties.deviceID;
  LOG(INFO) << "  * API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << "." << VK_VERSION_MINOR(properties.apiVersion)
            << "." << VK_VERSION_PATCH(properties.apiVersion);
}

static inline auto FindMemoryType(const VkPhysicalDevice& physical_device, const uint32_t filter,
                                  const VkMemoryPropertyFlags& flags) -> uint32_t {
  VkPhysicalDeviceMemoryProperties mem_properties{};
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);
  for (auto idx = 0; idx < mem_properties.memoryTypeCount; idx++) {
    if ((filter & (1 << idx)) && ((mem_properties.memoryTypes[idx].propertyFlags & flags) == flags))
      return idx;
  }
  LOG(FATAL) << "failed to find suitable memory for physical device.";
}

static inline void CopyVkBuffer(const VkDevice& device, const VkCommandPool& command_pool, const VkQueue& queue, VkBuffer& dst,
                                const VkBuffer& src, const VkDeviceSize size) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer{};
  CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &command_buffer), "failed to allocate vk command buffer");

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &begin_info);
  {
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;
    vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy_region);
  }
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  CHECK_VK(FATAL, vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE), "failed to submit to graphics queue");
  CHECK_VK(FATAL, vkQueueWaitIdle(queue), "failed to wait for graphics queue idle");
  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

static inline void InitVkBuffer(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkDeviceSize size,
                                const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, VkBuffer& buffer,
                                VkDeviceMemory& buffer_memory, const VkAllocationCallbacks* allocator = nullptr,
                                const VkDeviceSize offset = 0) {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  CHECK_VK(FATAL, vkCreateBuffer(device, &create_info, allocator, &buffer), "failed to create vk buffer");

  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  alloc_info.memoryTypeIndex = FindMemoryType(physical_device, mem_requirements.memoryTypeBits, properties);
  CHECK_VK(FATAL, vkAllocateMemory(device, &alloc_info, allocator, &buffer_memory), "failed to allocate vk memory");
  vkBindBufferMemory(device, buffer, buffer_memory, offset);
}

namespace vk {
class Buffer {
 public:
  static constexpr const VkMemoryPropertyFlags kDefaultBufferMemoryProperties =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  static void AllocateImageMemory(Driver* driver, const VkImage& image, VkDeviceMemory& bufferMemory,
                                  VkMemoryPropertyFlags properties);
  static void CopyDataToImageWithStaging(const VkImage& image, const uint8_t* data, const uint64_t num_bytes,
                                         const std::vector<VkBufferImageCopy>& regions);

 private:
  static void AllocateMemory(Driver* driver, const VkDeviceSize alloc_size, const uint32_t memory_type, VkDeviceMemory& memory);
  static void AllocateMemory(Driver* driver, const VkMemoryRequirements& mem_requirements, VkDeviceMemory& memory,
                             VkMemoryPropertyFlags properties);

  static inline void InitDescriptor(VkDescriptorBufferInfo& descriptor, const VkBuffer& buffer,
                                    const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0) {
    descriptor.buffer = buffer;
    descriptor.offset = offset;
    descriptor.range = size;
  }

 private:
  VkBuffer buffer_{};
  VkDeviceMemory memory_{};
  VkDeviceSize size_;
  VkDescriptorBufferInfo descriptor_{};

  explicit Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties);

 public:
  virtual ~Buffer();

  auto GetBuffer() const -> const VkBuffer& {
    return buffer_;
  }

  auto GetMemory() const -> const VkDeviceMemory& {
    return memory_;
  }

  auto GetSize() const -> const VkDeviceSize& {
    return size_;
  }

  auto GetDescriptor() const -> const VkDescriptorBufferInfo& {
    return descriptor_;
  }

  void Destroy();
  void Flush(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);
  void CopyFromBytes(const void* data, const uint64_t num_bytes, const bool staging = false);
  void CopyFromBuffer(const VkBuffer& src, const VkDeviceSize num_bytes = VK_WHOLE_SIZE);

  inline void CopyFromBuffer(const Buffer* buffer, const VkDeviceSize num_bytes = VK_WHOLE_SIZE) {
    ASSERT(buffer && buffer->GetMemory() != VK_NULL_HANDLE);
    return CopyFromBuffer(buffer->GetBuffer(), num_bytes);
  }

 public:
  static inline auto New(const VkDeviceSize size, const VkBufferUsageFlags usage,
                         const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return new Buffer(size, usage, properties);
  }

  static inline auto NewUniformBuffer(const VkDeviceSize size,
                                      const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return New(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, properties);
  }

  template <typename T>
  static inline auto NewUniformBuffer(const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return NewUniformBuffer(sizeof(T), properties);
  }

  static inline auto NewTransferSource(const VkDeviceSize size,
                                       const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return new Buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, properties);
  }

  static inline auto NewStaging(const VkDeviceSize size, const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return New(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, properties);
  }

  static inline auto NewVertex(const VkDeviceSize size, const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return New(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, properties);
  }

  static inline auto NewIndex(const VkDeviceSize size, const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties)
      -> Buffer* {
    return New(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, properties);
  }
};

class MappedBufferScope {
 private:
  const Buffer* buffer_;
  void* mapped_memory_ = nullptr;

 public:
  MappedBufferScope(const Buffer* buffer, const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0,
                    const VkMemoryMapFlags flags = 0);
  ~MappedBufferScope();

  auto GetBuffer() const -> const Buffer* {
    return buffer_;
  }

  auto GetMappedMemory() const -> void* {
    return mapped_memory_;
  }

  auto IsMapped() const -> bool {
    return GetMappedMemory() != nullptr;
  }

  void Flush(const VkDeviceSize num_bytes = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);
  void CopyFrom(const void* data, const VkDeviceSize num_bytes = VK_WHOLE_SIZE);

  operator bool() const {
    return IsMapped();
  }
};

using ValidationLayerList = std::vector<const char*>;
using ExtensionList = std::vector<const char*>;
}  // namespace vk

#endif  // PRT_DEBUG

namespace engine {
class InitState;
class TerminatedState;
}  // namespace engine

class Window;
class Driver : public DriverBase {
  friend class Runtime;
  friend class LuaState;
  friend class DriverBase;
  friend class InitState;
  friend class TerminatedState;
  DEFINE_NON_COPYABLE_TYPE(Driver);

 private:
  VkApplicationInfo app_info_{};
  VkInstance instance_{};
  VkPhysicalDevice physical_device_{};
  VkDevice device_{};
  VkQueue graphics_queue_{};
  VkQueue present_queue_{};
  VkSurfaceKHR surface_{};
  VkCommandPool command_pool_{};
  VkDescriptorPool descriptor_pool_{};
  VkAllocationCallbacks* allocator_ = nullptr;
  vk::ExtensionList instance_extensions_{};
  vk::ExtensionList device_extensions_{
      "VK_KHR_portability_subset",
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };
  vk::ValidationLayerList validation_layers_{"VK_LAYER_KHRONOS_validation"};
#ifdef PRT_DEBUG
  VkDebugUtilsMessengerEXT debug_{};
#endif  // PRT_DEBUG

  Driver();
  void InitApplicationInfo();
  void InitInstance();
  void InitPhysicalDevice();
  void InitSurface();
  void InitLogicalDevice(const float priority);
  void InitCommandPool();
  void InitDescriptorPool();
#ifdef PRT_DEBUG
  void InitDebugMessenger();
#endif  // PRT_DEBUG

 public:
  ~Driver();

  auto GetApplicationInfo() const -> VkApplicationInfo {
    return app_info_;
  }

  auto GetInstance() const -> VkInstance {
    return instance_;
  }

  auto GetPhysicalDevice() const -> VkPhysicalDevice {
    return physical_device_;
  }

  auto GetDevice() const -> VkDevice {
    return device_;
  }

  auto GetGraphicsQueue() const -> VkQueue {
    return graphics_queue_;
  }

  auto GetPresentQueue() const -> VkQueue {
    return present_queue_;
  }

  auto GetValidationLayers() const -> const vk::ValidationLayerList& {
    return validation_layers_;
  }

  auto GetInstanceExtensions() const -> const vk::ExtensionList& {
    return instance_extensions_;
  }

  auto GetDeviceExtensions() const -> const vk::ExtensionList& {
    return device_extensions_;
  }

  auto GetAllocator() const -> VkAllocationCallbacks* {
    return allocator_;
  }

  auto GetSurface() const -> const VkSurfaceKHR& {
    return surface_;
  }

  auto GetCommandPool() const -> VkCommandPool const& {
    return command_pool_;
  }

  auto GetDescriptorPool() const -> VkDescriptorPool const& {
    return descriptor_pool_;
  }

#ifdef PRT_DEBUG
  auto GetDebugMessenger() const -> VkDebugUtilsMessengerEXT {
    return debug_;
  }
#endif  // PRT_DEBUG

  void WaitDeviceIdle() const;
  auto GetDepthFormat() const -> VkFormat;
  void ReleaseCommandBuffers(const VkCommandBuffer* buffers, const uint64_t num_buffers) const;

  template <typename Container>
  inline void ReleaseCommandBuffers(const Container& buffers) const {
    if (buffers.empty()) {
      DLOG(WARNING) << "attempting to release 0 VkCommandBuffers.";
      return;
    }
    return ReleaseCommandBuffers(buffers.data(), buffers.size());
  }

  void CreateRenderPass(const VkRenderPassCreateInfo& create_info, VkRenderPass& pass);
  void CreatePipelineLayout(const VkPipelineLayoutCreateInfo& create_info, VkPipelineLayout& layout);
  void CreatePipelineCache(const VkPipelineCacheCreateInfo& create_info, VkPipelineCache& cache);
  void CreateGraphicsPipeline(const VkGraphicsPipelineCreateInfo& create_info, const VkPipelineCache& cache,
                              VkPipeline& pipeline);
  void AllocateCommandBuffers(const VkCommandBufferAllocateInfo& alloc_info, VkCommandBuffer* buffers);

  template <typename H>
  inline void Destroy(const H& handle, void (*destroyer)(VkDevice, H, const VkAllocationCallbacks*)) {
    if (handle != VK_NULL_HANDLE)
      return destroyer(GetDevice(), handle, GetAllocator());
  }

  void Destroy(const VkDeviceMemory& memory);

 private:
  static inline auto New() -> Driver* {
    ASSERT(!DriverBase::IsInitialized());
    return new Driver();
  }
};

class CommandBufferPool;
using CommandBufferPoolPtr = std::shared_ptr<CommandBufferPool>;
class CommandBufferPool {
 private:
  std::vector<VkCommandBuffer> buffers_{};

 public:
  CommandBufferPool() = default;
  explicit CommandBufferPool(const VkCommandPool& pool, const uint64_t num_buffers);
  ~CommandBufferPool();

  auto IsEmpty() const -> bool {
    return buffers_.empty();
  }

  auto GetNumberOfBuffers() const -> uint64_t {
    return buffers_.size();
  }

  auto GetBufferAt(const uint64_t idx) -> VkCommandBuffer& {
    ASSERT(idx >= 0 && idx <= GetNumberOfBuffers());
    return buffers_[idx];
  }

 public:
  static inline auto New(const VkCommandPool& pool, const uint64_t num_buffers) -> CommandBufferPool* {
    ASSERT(num_buffers > 0);
    return new CommandBufferPool(pool, num_buffers);
  }
};

class CommandBufferScope {
 private:
  VkCommandBufferBeginInfo begin_info_{};
  VkCommandBuffer& buffer_;

 public:
  explicit CommandBufferScope(VkCommandBuffer& buffer, const bool reset = false);
  ~CommandBufferScope();

  auto GetBeginInfo() const -> VkCommandBufferBeginInfo const& {
    return begin_info_;
  }

  operator VkCommandBuffer&() const {
    return buffer_;
  }
};

class BaseRenderPipeline;
class RenderPassScope {
 private:
  const VkCommandBuffer& buffer_;
  VkRenderPassBeginInfo begin_info_{};
  const VkRenderPass& pass_;

 public:
  explicit RenderPassScope(const VkCommandBuffer& buffer, const VkRenderPass& pass, const VkFramebuffer& framebuffer,
                           const std::vector<VkClearValue>& clear_values);
  ~RenderPassScope();

  auto GetBeginInfo() const -> VkRenderPassBeginInfo const& {
    return begin_info_;
  }

  void Bind(BaseRenderPipeline* pipeline);

  operator const VkRenderPass&() const {
    return pass_;
  }
};

class SingleUseCommandBuffer {
 private:
  VkCommandBuffer buffer_{};

  static void Allocate(const VkDevice& device, const VkCommandPool& pool, VkCommandBuffer& buffer);
  static void Start(const VkCommandBuffer& buffer);
  static void Finish(const VkCommandBuffer& buffer);
  static void Submit(const VkDevice& device, const VkQueue& queue, const VkCommandBuffer& buffer,
                     const VkAllocationCallbacks* allocator);
  static void Destroy(const VkDevice& device, const VkCommandPool& pool, const VkCommandBuffer& buffer);

 public:
  SingleUseCommandBuffer();
  ~SingleUseCommandBuffer();

  void Finish();

  operator VkCommandBuffer&() {
    return buffer_;
  }
};

namespace vk {
static inline auto NewImageView(const Driver* driver, const VkImage& image, const VkFormat& format,
                                const VkImageAspectFlags flags) -> VkImageView {
  VkImageViewCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  create_info.image = image;
  create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  create_info.format = format;
  create_info.subresourceRange.aspectMask = flags;
  create_info.subresourceRange.baseMipLevel = 0;
  create_info.subresourceRange.levelCount = 1;
  create_info.subresourceRange.baseArrayLayer = 0;
  create_info.subresourceRange.layerCount = 1;

  VkImageView view{};
  CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &create_info, driver->GetAllocator(), &view),
           "failed to create vk image view");
  return view;
}

void InitFramebuffers(const Driver* driver, const VkRenderPass& pass, const VkExtent2D& extent,
                      std::vector<VkFramebuffer>& framebuffers);
}  // namespace vk
}  // namespace prt

#endif  // PRT_GFX_VK_H
