#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/lua.h"
#include "prette/renderer.h"
#include "prette/scene_renderer.h"
#include "prette/to_string.h"
#include "prette/window.h"

namespace prt {
namespace vk {
void Buffer::AllocateMemory(Driver* driver, const VkDeviceSize alloc_size, const uint32_t memory_type, VkDeviceMemory& memory) {
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = alloc_size;
  alloc_info.memoryTypeIndex = memory_type;
  CHECK_VK(FATAL, vkAllocateMemory(driver->GetDevice(), &alloc_info, driver->GetAllocator(), &memory),
           "failed to allocate vk memory");
  ASSERT(memory != VK_NULL_HANDLE);
}

void Buffer::AllocateMemory(Driver* driver, const VkMemoryRequirements& mem_requirements, VkDeviceMemory& memory,
                            VkMemoryPropertyFlags properties) {
  const auto memory_type_index = FindMemoryType(driver->GetPhysicalDevice(), mem_requirements.memoryTypeBits, properties);
  return AllocateMemory(driver, mem_requirements.size, memory_type_index, memory);
}

void Buffer::AllocateImageMemory(Driver* driver, const VkImage& image, VkDeviceMemory& memory, VkMemoryPropertyFlags properties) {
  VkMemoryRequirements mem_requirements{};
  vkGetImageMemoryRequirements(driver->GetDevice(), image, &mem_requirements);
  AllocateMemory(driver, mem_requirements, memory, properties);
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
  CHECK_VK(FATAL, vkCreateBuffer(device, &create_info, driver->GetAllocator(), &buffer_), "failed to create vk buffer");
  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device, buffer_, &mem_requirements);
  AllocateMemory(driver, mem_requirements, memory_, properties);
  CHECK_VK(FATAL, vkBindBufferMemory(device, buffer_, memory_, 0), "failed to bind vk buffer memory");
  InitDescriptor(descriptor_, buffer_);
}

Buffer::~Buffer() {
  Destroy();
}

class StagingBufferScope {
 private:
  Buffer* buffer_;
  MappedBufferScope mapped_;

 public:
  StagingBufferScope(const uint64_t num_bytes) :
    buffer_(Buffer::NewTransferSource(num_bytes)),
    mapped_(buffer_) {
    ASSERT(buffer_);
  }
  ~StagingBufferScope() {
    mapped_.~MappedBufferScope();
    delete buffer_;
  }

  auto GetBuffer() const -> Buffer* {
    return buffer_;
  }

  inline auto HasBuffer() const -> bool {
    return GetBuffer() != nullptr;
  }

  inline auto IsMapped() const -> bool {
    return mapped_;
  }

  void CopyFrom(const void* src, const uint64_t num_bytes) {
    ASSERT(IsMapped());
    ASSERT(num_bytes >= 0);  // TODO: check upper bounds
    return mapped_.CopyFrom(src, num_bytes);
  }

  operator bool() const {
    return HasBuffer() && IsMapped();
  }

  operator VkBuffer const&() const {
    ASSERT(buffer_);
    return buffer_->GetBuffer();
  }
};

void Buffer::CopyDataToImageWithStaging(const VkImage& image, const uint8_t* data, const uint64_t num_bytes,
                                        const std::vector<VkBufferImageCopy>& regions) {
  StagingBufferScope staging(num_bytes);
  staging.CopyFrom(data, num_bytes);

  SingleUseCommandBuffer cmds(Renderer::GetCommandPool());
  vkCmdCopyBufferToImage(cmds, staging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(regions.size()),
                         regions.data());
}

void Buffer::CopyFromBytes(const void* data, const uint64_t num_bytes, const bool staging) {
  ASSERT(data);
  ASSERT(num_bytes >= 1);
  ASSERT(memory_ != VK_NULL_HANDLE);
  if (staging) {
    StagingBufferScope staging(num_bytes);
    ASSERT(staging);
    staging.CopyFrom(data, num_bytes);
    CopyFromBuffer(staging.GetBuffer(), num_bytes);
  } else {
    MappedBufferScope mapped(this);
    ASSERT(mapped);
    mapped.CopyFrom(data, num_bytes);
  }
}

void Buffer::CopyFromBuffer(const VkBuffer& src, const VkDeviceSize num_bytes) {
  SingleUseCommandBuffer buffer(SceneRenderer::GetCommandPool());
  VkBufferCopy copy{};
  copy.size = num_bytes;
  vkCmdCopyBuffer(buffer, src, buffer_, 1, &copy);
}

void Buffer::Destroy() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  if (buffer_)
    vkDestroyBuffer(driver->GetDevice(), buffer_, nullptr);
  if (memory_)
    vkFreeMemory(driver->GetDevice(), memory_, nullptr);
}

void Buffer::Flush(const VkDeviceSize size, const VkDeviceSize offset) {
  VkMappedMemoryRange range{};
  range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.memory = memory_;
  range.offset = offset;
  range.size = size;
  const auto driver = Driver::Get();
  ASSERT(driver);
  CHECK_VK(FATAL, vkFlushMappedMemoryRanges(driver->GetDevice(), 1, &range), "failed to flush vk buffer");
}

MappedBufferScope::MappedBufferScope(const Buffer* buffer, const VkDeviceSize size, const VkDeviceSize offset,
                                     const VkMemoryMapFlags flags) :
  buffer_(buffer) {
  ASSERT(buffer_);
  const auto driver = Driver::Get();
  ASSERT(driver);
  CHECK_VK(FATAL, vkMapMemory(driver->GetDevice(), GetBuffer()->GetMemory(), offset, size, flags, &mapped_memory_),
           "failed to map vk buffer memory");
}

MappedBufferScope::~MappedBufferScope() {
  if (!IsMapped())
    return;
  ASSERT(GetBuffer()->GetMemory() != VK_NULL_HANDLE);
  const auto driver = Driver::Get();
  ASSERT(driver);
  vkUnmapMemory(driver->GetDevice(), GetBuffer()->GetMemory());
  mapped_memory_ = nullptr;
}

void MappedBufferScope::CopyFrom(const void* src, const VkDeviceSize num_bytes) {
  memcpy(mapped_memory_, src, num_bytes == VK_WHOLE_SIZE ? GetBuffer()->GetSize() : num_bytes);
}

void MappedBufferScope::Flush(const VkDeviceSize num_bytes, const VkDeviceSize offset) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  VkMappedMemoryRange range{};
  range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  range.memory = buffer_->GetMemory();
  range.offset = offset;
  range.size = num_bytes;
  CHECK_VK(FATAL, vkFlushMappedMemoryRanges(driver->GetDevice(), 1, &range), "failed to flush mapped vk memory range");
}

void InitFramebuffers(const Driver* driver, const VkRenderPass& pass, const std::vector<VkImageView>& views,
                      const VkExtent2D& extent, std::vector<VkFramebuffer>& framebuffers) {
  ASSERT(driver);
  framebuffers.resize(views.size());
  for (auto idx = 0; idx < views.size(); idx++) {
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = pass;
    create_info.attachmentCount = 1;
    create_info.pAttachments = &views[idx];
    create_info.width = extent.width;
    create_info.height = extent.height;
    create_info.layers = 1;
    CHECK_VK(FATAL, vkCreateFramebuffer(driver->GetDevice(), &create_info, driver->GetAllocator(), &framebuffers[idx]),
             "failed to create vk framebuffers");
  }
}
}  // namespace vk

static inline void GetRequiredExtensions(std::vector<const char*>& extensions) {
  uint32_t ext_count = 0;
  const auto glfw_exts = glfwGetRequiredInstanceExtensions(&ext_count);
  extensions.insert(std::end(extensions), glfw_exts, glfw_exts + ext_count);
#ifdef OS_IS_OSX
  extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif  // OS_IS_OSX

#ifdef PRT_DEBUG
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif  // PRT_DEBUG
}

#ifdef PRT_DEBUG
static VKAPI_ATTR auto VKAPI_CALL OnDebugCreateInfo(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
    -> VkBool32 {
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

template <typename F, typename... Args>
static inline auto CallVkIfExists(VkInstance& instance, const char* name, Args... args) -> VkResult {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  return func ? func(instance, args...) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

template <typename F, typename... Args>
static inline void InvokeVkIfExists(VkInstance& instance, const char* name, Args... args) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const auto func = (F)vkGetInstanceProcAddr(instance, name);
  if (func)
    func(instance, args...);
}

static constexpr const auto kCreateDebugUtilsMessengerEXTName = "vkCreateDebugUtilsMessengerEXT";
VKAPI_ATTR auto VKAPI_CALL CreateDebugUtilsMessengerEXT(VkInstance& instance,
                                                        const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                                        const VkAllocationCallbacks* allocator,
                                                        VkDebugUtilsMessengerEXT* messenger) -> VkResult {
  const auto result = CallVkIfExists<PFN_vkCreateDebugUtilsMessengerEXT>(instance, kCreateDebugUtilsMessengerEXTName, create_info,
                                                                         allocator, messenger);
  if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
    DLOG(WARNING) << "vk extension `" << kCreateDebugUtilsMessengerEXTName << "` is not present, skipping debug messenger.";
    return VK_SUCCESS;
  }
  return result;
}

static constexpr const auto kDestroyDebugUtilsMessengerEXTName = "vkDestroyDebugUtilsMessengerEXT";
VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(VkInstance& instance, const VkDebugUtilsMessengerEXT messenger,
                                                         const VkAllocationCallbacks* allocator = nullptr) {
  return InvokeVkIfExists<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, kDestroyDebugUtilsMessengerEXTName, messenger,
                                                               allocator);
}

static inline void InitDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info,
                                                PFN_vkDebugUtilsMessengerCallbackEXT callback) {
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = callback;
  info.pNext = nullptr;
}
#endif  // PRT_DEBUG

static inline auto HasValidationLayerSupport(const std::vector<const char*>& layers) -> bool {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);

  std::vector<VkLayerProperties> available(count);
  vkEnumerateInstanceLayerProperties(&count, &available[0]);

  for (const auto& name : layers) {
    const auto pos = std::ranges::find_if(available, [name](const VkLayerProperties& props) {
      return strcmp(name, props.layerName) == 0;
    });
    if (pos == std::end(available))
      return false;
  }
  return true;
}

static inline auto CheckDeviceExtensionSupport(const VkPhysicalDevice& device, const std::vector<const char*>& extensions)
    -> bool {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> available(count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &count, &available[0]);

  std::unordered_set<std::string> required(std::begin(extensions), std::end(extensions));
  for (const auto& ext : available) {
    required.erase(ext.extensionName);
  }
  return required.empty();
}

static inline auto IsDeviceSuitable(const VkSurfaceKHR& surface, const std::vector<const char*>& extensions)
    -> std::function<bool(const VkPhysicalDevice& device)> {
  return [&surface, &extensions](const VkPhysicalDevice& device) {
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    const auto extensions_supported = CheckDeviceExtensionSupport(device, extensions);
    bool swap_supported = false;
    if (extensions_supported) {
      swap_supported = QuerySwapChainSupport(device, surface);
    }
    return indices.IsComplete() && extensions_supported && swap_supported;
  };
}

void Driver::InitPhysicalDevice() {
  LOG_IF(FATAL, !FindSuitablePhysicalDevice(instance_, &physical_device_, IsDeviceSuitable(surface_, device_extensions_)))
      << "failed to find suitable GPU w/ vulkan support.";
#ifdef PRT_DEBUG
  LOG(INFO) << "found suitable GPU w/ vulkan support:";
  PrintProperties(physical_device_);
#endif  // PRT_DEBUG
  Publish<PhysicalDeviceInitEvent>(this);
}

#ifdef PRT_DEBUG

void Driver::InitSurface() {
  const auto window = GetAppWindow();
  ASSERT(window);
  CHECK_VK(FATAL, glfwCreateWindowSurface(instance_, window->GetHandle(), allocator_, &surface_),
           "failed to create window vk surface");
  Publish<SurfaceInitEvent>(this);
}

void Driver::InitDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
  InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
  CHECK_VK(FATAL, CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr, &debug_), "failed to create vk debug messenger");
}

#endif  // PRT_DEBUG

void Driver::InitApplicationInfo() {
  app_info_.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info_.pApplicationName = "Prette";
  app_info_.applicationVersion = VK_MAKE_PRT_VERSION;
  app_info_.pEngineName = "Prette";
  app_info_.engineVersion = VK_MAKE_PRT_VERSION;
  app_info_.pNext = nullptr;
  app_info_.apiVersion = VK_API_VERSION_1_3;
}

void Driver::WaitDeviceIdle() {
  DLOG(INFO) << "waiting for idle...";
  vkDeviceWaitIdle(device_);
}

void Driver::InitInstance() {
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info_;
  create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  create_info.enabledLayerCount = 0;

  create_info.enabledExtensionCount = instance_extensions_.size();
  create_info.ppEnabledExtensionNames = &instance_extensions_[0];

  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
#ifdef PRT_DEBUG
  create_info.enabledLayerCount = validation_layers_.size();
  create_info.ppEnabledLayerNames = &validation_layers_[0];
  InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
  create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info;
#else
  info.enabledLayerCount = 0;
  info.pNext = nullptr;
#endif  // PRT_DEBUG
  CHECK_VK(FATAL, vkCreateInstance(&create_info, nullptr, &instance_), "failed to create vkInstance");
  Publish<InstanceInitEvent>(this);
}

void Driver::InitLogicalDevice(const float priority) {
  const auto indices = FindQueueFamilies(physical_device_, surface_);
  std::unordered_set<uint32_t> unique_families{};
  indices.GetUniqueFamilies(unique_families);
  std::vector<VkDeviceQueueCreateInfo> create_infos{};

  for (const auto& family : unique_families) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &priority;
    create_infos.push_back(queue_create_info);
  }

  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &create_infos[0];
  create_info.queueCreateInfoCount = create_infos.size();
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledExtensionCount = device_extensions_.size();
  create_info.ppEnabledExtensionNames = &device_extensions_[0];

#ifdef PRT_DEBUG
  create_info.enabledLayerCount = validation_layers_.size();
  create_info.ppEnabledLayerNames = &validation_layers_[0];
#else
  create_info.enabledLayerCount = 0;
#endif  // PRT_DEBUG
  CHECK_VK(FATAL, vkCreateDevice(physical_device_, &create_info, nullptr, &device_), "failed to create vk device");
  indices.GetGraphicsQueue(device_, graphics_queue_);
  indices.GetPresentQueue(device_, present_queue_);
  Publish<DeviceInitEvent>(this);
}

Driver::Driver() {
#ifdef PRT_DEBUG
  LOG_IF(FATAL, !HasValidationLayerSupport(validation_layers_)) << "vk validation layers requested but not available!";
#endif  // PRT_DEBUG
  GetRequiredExtensions(instance_extensions_);
  InitApplicationInfo();
  InitInstance();
  InitSurface();
  InitPhysicalDevice();
  InitLogicalDevice(1.0f);
}

Driver::~Driver() {
  vkDestroySurfaceKHR(instance_, surface_, allocator_);
  vkDestroyDevice(device_, allocator_);
#ifdef PRT_DEBUG
  DestroyDebugUtilsMessengerEXT(instance_, debug_);
#endif  // PRT_DEBUG
  vkDestroyInstance(instance_, allocator_);
}

void Driver::Init() {
  DriverBase::Init();
  const auto engine = Engine::Get();
  ASSERT(engine);
  engine::OnTerminatingEvent().subscribe([](engine::TerminatingEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    driver->WaitDeviceIdle();
  });
}

void SingleUseCommandBuffer::Allocate(const VkDevice& device, const VkCommandPool& command_pool, VkCommandBuffer& buffer) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = command_pool;
  alloc_info.commandBufferCount = 1;
  CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &buffer), "failed to allocate single use vk command buffer");
}

void SingleUseCommandBuffer::Start(const VkCommandBuffer& buffer) {
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin vk command buffer");
}

void SingleUseCommandBuffer::Finish(const VkCommandBuffer& buffer) {
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end single use vk command buffer");
}

void SingleUseCommandBuffer::Destroy(const VkDevice& device, const VkCommandPool& pool, const VkCommandBuffer& buffer) {
  vkFreeCommandBuffers(device, pool, 1, &buffer);
}

void SingleUseCommandBuffer::Submit(const VkDevice& device, const VkQueue& queue, const VkCommandBuffer& buffer,
                                    const VkAllocationCallbacks* allocator) {
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &buffer;

  VkFenceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  create_info.flags = VK_FLAGS_NONE;

  VkFence fence{};
  CHECK_VK(FATAL, vkCreateFence(device, &create_info, allocator, &fence), "failed to create vk fence");
  vkQueueSubmit(queue, 1, &submit_info, fence);
  CHECK_VK(FATAL, vkWaitForFences(device, 1, &fence, VK_TRUE, VK_DEFAULT_FENCE_TIMEOUT), "failed to wait for vk fence");
  vkDestroyFence(device, fence, allocator);
}

static inline void DestroyCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool,
                                        const VkCommandBuffer& buffer) {
  vkFreeCommandBuffers(device, command_pool, 1, &buffer);
}

SingleUseCommandBuffer::SingleUseCommandBuffer(const VkCommandPool& pool) :
  pool_(pool) {
  const auto driver = Driver::Get();
  ASSERT(driver);
  Allocate(driver->GetDevice(), pool_, buffer_);
  Start(buffer_);
}

SingleUseCommandBuffer::~SingleUseCommandBuffer() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  CHECK_VK(FATAL, vkEndCommandBuffer(buffer_), "failed to end vk command buffer");
  Submit(driver->GetDevice(), driver->GetGraphicsQueue(), buffer_, driver->GetAllocator());
  Destroy(driver->GetDevice(), pool_, buffer_);
}
}  // namespace prt