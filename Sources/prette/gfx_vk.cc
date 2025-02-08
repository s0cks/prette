#include <vulkan/vulkan_core.h>

#include "prette/command_pool.h"
#include "prette/common.h"
#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/to_string.h"
#ifdef PRT_VK

#include "prette/command_pool.h"
#include "prette/pipeline.h"
#include "prette/renderer.h"
#include "prette/swap_chain.h"
#include "prette/window.h"

namespace prt {

namespace vk {
void Buffer::AllocateMemory(const VkDevice& device, const VkDeviceSize alloc_size, const uint32_t memory_type,
                            VkDeviceMemory& memory, const VkAllocationCallbacks* allocator) {
  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = alloc_size;
  alloc_info.memoryTypeIndex = memory_type;
  CHECK_VK(FATAL, vkAllocateMemory(device, &alloc_info, nullptr, &memory), "failed to allocate vk memory");
  ASSERT(memory != VK_NULL_HANDLE);
}

Buffer::Buffer(const VkDeviceSize size, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) :
  size_(size) {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = usage;
  create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  const auto driver = GetDriver();
  ASSERT(driver);

  const auto& device = driver->GetDevice();
  CHECK_VK(FATAL, vkCreateBuffer(device, &create_info, nullptr, &buffer_), "failed to create vk buffer");

  VkMemoryRequirements mem_requirements{};
  vkGetBufferMemoryRequirements(device, buffer_, &mem_requirements);
  const auto memory_type = FindMemoryType(driver->GetPhysicalDevice(), mem_requirements.memoryTypeBits, properties);
  AllocateMemory(device, size, memory_type, memory_, nullptr);
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
};

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

class SingleUseCommandBuffer {
 private:
  VkCommandBuffer buffer_{};

  static inline void InitCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool, VkCommandBuffer& buffer) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = command_pool;
    alloc_info.commandBufferCount = 1;
    CHECK_VK(FATAL, vkAllocateCommandBuffers(device, &alloc_info, &buffer), "failed to allocate single use vk command buffer");
  }

  static inline void StartCommandBuffer(const VkCommandBuffer& buffer) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    CHECK_VK(FATAL, vkBeginCommandBuffer(buffer, &begin_info), "failed to begin vk command buffer");
  }

  static inline void SubmitCommandBuffer(const VkQueue& queue, const VkCommandBuffer& buffer) {
    CHECK_VK(FATAL, vkEndCommandBuffer(buffer), "failed to end vk command buffer");

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
  }

  static inline void DestroyCommandBuffer(const VkDevice& device, const VkCommandPool& command_pool,
                                          const VkCommandBuffer& buffer) {
    vkFreeCommandBuffers(device, command_pool, 1, &buffer);
  }

 public:
  SingleUseCommandBuffer() {
    const auto driver = GetDriver();
    ASSERT(driver);
    InitCommandBuffer(driver->GetDevice(), CommandPool::GetCommandPool(), buffer_);
    StartCommandBuffer(buffer_);
  }
  ~SingleUseCommandBuffer() {
    const auto driver = GetDriver();
    ASSERT(driver);
    SubmitCommandBuffer(driver->GetGraphicsQueue(), buffer_);
    DestroyCommandBuffer(driver->GetDevice(), CommandPool::GetCommandPool(), buffer_);
  }

  operator VkCommandBuffer() const {
    return buffer_;
  }
};

void Buffer::CopyFromBuffer(const VkBuffer& src, const VkDeviceSize num_bytes) {
  SingleUseCommandBuffer buffer;
  VkBufferCopy copy{};
  copy.size = num_bytes;
  vkCmdCopyBuffer(buffer, src, buffer_, 1, &copy);
}

void Buffer::Destroy() {
  const auto driver = GetDriver();
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
  const auto driver = GetDriver();
  ASSERT(driver);
  CHECK_VK(FATAL, vkFlushMappedMemoryRanges(driver->GetDevice(), 1, &range), "failed to flush vk buffer");
}

MappedBufferScope::MappedBufferScope(const Buffer* buffer, const VkDeviceSize size, const VkDeviceSize offset,
                                     const VkMemoryMapFlags flags) :
  buffer_(buffer) {
  ASSERT(buffer_);
  const auto driver = GetDriver();
  ASSERT(driver);
  CHECK_VK(FATAL, vkMapMemory(driver->GetDevice(), GetBuffer()->GetMemory(), offset, size, flags, &mapped_memory_),
           "failed to map vk buffer memory");
}

MappedBufferScope::~MappedBufferScope() {
  if (!IsMapped())
    return;
  ASSERT(GetBuffer()->GetMemory() != VK_NULL_HANDLE);
  const auto driver = GetDriver();
  ASSERT(driver);
  vkUnmapMemory(driver->GetDevice(), GetBuffer()->GetMemory());
  mapped_memory_ = nullptr;
}

void MappedBufferScope::CopyFrom(const void* src, const VkDeviceSize num_bytes) {
  memcpy(mapped_memory_, src, num_bytes == VK_WHOLE_SIZE ? GetBuffer()->GetSize() : num_bytes);
}
}  // namespace vk

static DriverEventSubject driver_events_;
static VulkanDriver* driver_ = nullptr;

static inline void PublishDriverEvent(DriverEvent* event) {
  ASSERT(event);
  const auto& subscriber = driver_events_.get_subscriber();
  subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void PublishDriverEvent(Args... args) {
  E event(args...);
  return PublishDriverEvent((DriverEvent*)&event);
}

auto OnDriverEvent() -> DriverEventObservable {
  return driver_events_.get_observable();
}

auto DriverCreatedEvent::ToString() const -> std::string {
  ToStringHelper<DriverCreatedEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto DriverDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<DriverDestroyedEvent>{};
}

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
  LOG(ERROR) << "Validation Layer: " << callbackData->pMessage;
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

void VulkanDriver::InitPhysicalDevice() {
  LOG_IF(FATAL, !FindSuitablePhysicalDevice(instance_, &physical_device_, IsDeviceSuitable(surface_, device_extensions_)))
      << "failed to find suitable GPU w/ vulkan support.";
#ifdef PRT_DEBUG
  LOG(INFO) << "found suitable GPU w/ vulkan support:";
  PrintProperties(physical_device_);
#endif  // PRT_DEBUG
}

#ifdef PRT_DEBUG

void VulkanDriver::InitSurface() {
  const auto window = GetAppWindow();
  ASSERT(window);
  CHECK_VK(FATAL, glfwCreateWindowSurface(instance_, window->GetHandle(), allocator_, &surface_),
           "failed to create window vk surface");
}

void VulkanDriver::InitDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
  InitDebugMessengerCreateInfo(debug_info, &OnDebugCreateInfo);
  CHECK_VK(FATAL, CreateDebugUtilsMessengerEXT(instance_, &debug_info, nullptr, &debug_), "failed to create vk debug messenger");
}

#endif  // PRT_DEBUG

void VulkanDriver::InitApplicationInfo() {
  app_info_.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info_.pApplicationName = "Hello World";
  app_info_.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  app_info_.pEngineName = "No Engine";
  app_info_.engineVersion = VK_MAKE_VERSION(0, 0, 0);
  app_info_.pNext = nullptr;
  app_info_.apiVersion = VK_API_VERSION_1_3;
}

void VulkanDriver::WaitDeviceIdle() {
  vkDeviceWaitIdle(device_);
}

void VulkanDriver::InitInstance() {
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
}

void VulkanDriver::InitLogicalDevice(const float priority) {
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
}

VulkanDriver::VulkanDriver() {
#ifdef PRT_DEBUG
  LOG_IF(FATAL, !HasValidationLayerSupport(validation_layers_)) << "vk validation layers requested but not available!";
#endif  // PRT_DEBUG
  GetRequiredExtensions(instance_extensions_);
  InitApplicationInfo();
  InitInstance();
  InitSurface();
  InitPhysicalDevice();
  InitLogicalDevice(1.0f);
  const auto engine = Engine::Get();
  ASSERT(engine);
  engine->OnTerminatingEvent().subscribe([this](engine::TerminatingEvent* event) {
    ASSERT(event);
    Destroy();
  });
}

VulkanDriver::~VulkanDriver() {
  vkDestroySurfaceKHR(instance_, surface_, allocator_);
  vkDestroyDevice(device_, allocator_);
#ifdef PRT_DEBUG
  DestroyDebugUtilsMessengerEXT(instance_, debug_);
#endif  // PRT_DEBUG
  vkDestroyInstance(instance_, allocator_);
}

auto VulkanDriver::New() -> VulkanDriver* {
  const auto driver = new VulkanDriver();
  ASSERT(driver);
  PublishDriverEvent<DriverCreatedEvent>(driver);
  return driver;
}

void VulkanDriver::Destroy() {
  ASSERT(driver_);
  driver_->WaitDeviceIdle();
  Renderer::Shutdown(driver_);
  SwapChain::Shutdown(driver_);
  CommandPool::Shutdown(driver_);
  Pipeline::Shutdown(driver_);
  delete driver_;
  driver_ = nullptr;
  PublishDriverEvent<DriverDestroyedEvent>();
}

auto InitDriver() -> Driver* {
  ASSERT(driver_ == nullptr);
#ifdef PRT_DEBUG
  OnDriverEvent().subscribe(LogEvent<DriverEvent>(google::INFO, __FILE__, __LINE__));
#endif  // PRT_DEBUG
  driver_ = VulkanDriver::New();
  ASSERT(driver_);
  return driver_;
}

auto GetDriver() -> Driver* {
  ASSERT(driver_);
  return driver_;
}
}  // namespace prt

#endif  // PRT_VK