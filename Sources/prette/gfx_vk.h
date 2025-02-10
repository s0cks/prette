#ifndef PRT_GFX_H
#error "Please #include <prt/gfx.h> instead."
#endif  // PRT_GFX_H

#ifndef PRT_GFX_VK_H
#define PRT_GFX_VK_H

#include <glog/logging.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <unordered_set>
#include <vector>

#include "prette/common.h"
#include "prette/lua.h"
#include "prette/prette.h"

namespace prt {
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

 private:
  static void AllocateMemory(const VkDevice& device, const VkDeviceSize alloc_size, const uint32_t memory_type,
                             VkDeviceMemory& memory, const VkAllocationCallbacks* allocator = nullptr);

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

  static inline auto NewTransferSource(const VkDeviceSize size,
                                       const VkMemoryPropertyFlags properties = kDefaultBufferMemoryProperties) -> Buffer* {
    return new Buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, properties);
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
class VulkanDriver : public DriverBase {
  friend class Runtime;
  friend class LuaState;
  friend class DriverBase;
  friend class engine::InitState;
  friend class engine::TerminatedState;
  DEFINE_NON_COPYABLE_TYPE(VulkanDriver);

 private:
  VkApplicationInfo app_info_{};
  VkInstance instance_{};
  VkPhysicalDevice physical_device_{};
  VkDevice device_{};
  VkQueue graphics_queue_{};
  VkQueue present_queue_{};
  VkSurfaceKHR surface_{};
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

  VulkanDriver();
  void InitApplicationInfo();
  void InitInstance();
  void InitPhysicalDevice();
  void InitSurface();
  void InitLogicalDevice(const float priority);
#ifdef PRT_DEBUG
  void InitDebugMessenger();
#endif  // PRT_DEBUG

 public:
  ~VulkanDriver();

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

#ifdef PRT_DEBUG
  auto GetDebugMessenger() const -> VkDebugUtilsMessengerEXT {
    return debug_;
  }
#endif  // PRT_DEBUG

  void WaitDeviceIdle();

 private:
  static inline auto New() -> VulkanDriver* {
    ASSERT(!DriverBase::IsInitialized());
    return new VulkanDriver();
  }

 public:
  static void Init();
};
}  // namespace prt

#endif  // PRT_GFX_VK_H
