#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vector>

#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/tick.h"

namespace prt {
#define FOR_EACH_RENDERER_EVENT(V) \
  V(RendererCreated)               \
  V(PreFrame)                      \
  V(PostFrame)                     \
  V(RendererDestroyed)

class Renderer;
class RendererEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_RENDERER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class RendererEvent : public Event {
 public:
  RendererEvent() = default;
  ~RendererEvent() override = default;
  virtual auto IsFrameEvent() const -> bool = 0;
  DEFINE_EVENT_PROTOTYPE(Renderer, FOR_EACH_RENDERER_EVENT);
};

template <const bool FrameEvent>
class TemplateRendererEvent : public RendererEvent {
 public:
  TemplateRendererEvent() = default;
  ~TemplateRendererEvent() override = default;

  auto IsFrameEvent() const -> bool override {
    return FrameEvent;
  }
};

class PreFrameEvent : public TemplateRendererEvent<true> {
 public:
  PreFrameEvent() = default;
  ~PreFrameEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, PreFrame);
};

class PostFrameEvent : public TemplateRendererEvent<true> {
 public:
  PostFrameEvent() = default;
  ~PostFrameEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, PostFrame);
};

class RendererEventBase : public TemplateRendererEvent<false> {
 private:
  const Renderer* renderer_;

 protected:
  explicit RendererEventBase(const Renderer* renderer) :
    TemplateRendererEvent<false>(),
    renderer_(renderer) {
    ASSERT(renderer_);
  }

 public:
  ~RendererEventBase() override = default;
};

class RendererCreatedEvent : public TemplateRendererEvent<false> {
 public:
  RendererCreatedEvent() = default;
  ~RendererCreatedEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, RendererCreated);
};

class RendererDestroyedEvent : public TemplateRendererEvent<false> {
 public:
  RendererDestroyedEvent() = default;
  ~RendererDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, RendererDestroyed);
};

DEFINE_EVENT_SUBJECT(Renderer);
DEFINE_EVENT_OBSERVABLE(Renderer);
FOR_EACH_RENDERER_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnRendererEvent() -> RendererEventObservable;
#define DEFINE_ON_EVENT(Name)                                                    \
  static inline auto On##Name##Event()->Name##EventObservable {                  \
    return OnRendererEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;

  static auto GetBindingDescription() -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static auto GetAttributeDescriptions() -> const std::array<VkVertexInputAttributeDescription, 2>& {
    static std::array<VkVertexInputAttributeDescription, 2> attributes{};
    if (attributes.at(0).format != VK_FORMAT_R32G32_SFLOAT) {
      attributes.at(0).binding = 0;
      attributes.at(0).location = 0;
      attributes.at(0).format = VK_FORMAT_R32G32_SFLOAT;
      attributes.at(0).offset = offsetof(Vertex, pos);

      attributes.at(1).binding = 0;
      attributes.at(1).location = 1;
      attributes.at(1).format = VK_FORMAT_R32G32B32_SFLOAT;
      attributes.at(1).offset = offsetof(Vertex, color);
    }
    return attributes;
  }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR surface_capabilities{};
  std::vector<VkSurfaceFormatKHR> surface_formats{};
  std::vector<VkPresentModeKHR> present_modes{};

  inline auto HasSurfaceFormats() const -> bool {
    return !surface_formats.empty();
  }

  inline auto HasPresentModes() const -> bool {
    return !present_modes.empty();
  }

  auto FindSurfaceFormat(const std::function<bool(const VkSurfaceFormatKHR&)>& filter) const -> const VkSurfaceFormatKHR& {
    const auto pos = std::ranges::find_if(surface_formats, filter);
    if (pos != std::end(surface_formats))
      return (*pos);
    DLOG(WARNING) << "failed to find valid surface format for swap chain.";
    return surface_formats[0];
  }

  auto FindPresentMode(const std::function<bool(const VkPresentModeKHR&)>& filter) const -> VkPresentModeKHR {
    const auto pos = std::ranges::find_if(present_modes, filter);
    if (pos != std::end(present_modes))
      return (*pos);
    DLOG(WARNING) << "failed to find valid present mode for swap chain.";
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  auto GetMaxImageCount() const -> uint32_t {
    return surface_capabilities.maxImageCount;
  }

  inline auto HasMaxImage() const -> bool {
    return GetMaxImageCount() > 0;
  }

  void ClampImageCount(uint32_t& image_count) const {
    if (HasMaxImage() && image_count > GetMaxImageCount())
      image_count = GetMaxImageCount();
  }

  auto GetExtent() const -> VkExtent2D;

  operator bool() const {
    return HasSurfaceFormats() && HasPresentModes();
  }
};

auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> SwapChainSupportDetails;

static inline auto HasSwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> bool {
  return QuerySwapChainSupport(device, surface);
}

class Renderer {
  friend class LuaState;

 private:
  static void InitSyncObjects(const Driver* driver);
  static void InitResizeListener();
  static void InitBuffers();
  static void InitPipeline(const Driver* driver);
  static void InitPipelineCache(const Driver* driver);

  static void InitCommandPool(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                              const VkAllocationCallbacks* allocator);
  static void InitGuiCommandPool(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                                 const VkAllocationCallbacks* allocator);
  static void InitCommandBuffers(const VkDevice& device);
  static void InitGuiCommandBuffers(const VkDevice& device);

  static void InitImageViews(const VkDevice& device);

  static void InitFramebuffers(const Driver* driver, std::vector<VkFramebuffer>& framebuffers);
  static void InitRenderPass(const VkDevice& device);
  static void InitGuiRenderPass(const VkDevice& device);

  static void InitSwapChain(const VkPhysicalDevice& physical_device, const VkDevice& device, const VkSurfaceKHR& surface,
                            const VkAllocationCallbacks* allocator);
  static void ReInitSwapChain(Driver* driver);

  static void Destroy();
  static void DestroyBuffers();
  static void DestroyFramebuffers(Driver* driver);
  static void DestroyImageViews(Driver* driver);
  static void DestroyRenderPass(Driver* driver);
  static void DestroySwapChain(Driver* driver);
  static void DestroySyncObjects(Driver* driver);
  static void DestroyPipelineCache(Driver* driver);
  static void DestroyPipeline(Driver* driver);
  static void DestroyPipelineLayout(Driver* driver);

 private:
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static auto GetPipeline() -> VkPipeline const&;
  static auto GetPipelineLayout() -> VkPipelineLayout const&;
  static auto GetVertexBuffer() -> VkBuffer const&;
  static auto GetIndexBuffer() -> VkBuffer const&;
  static auto GetPipelineCache() -> VkPipelineCache const&;
  static auto GetNumberOfIndices() -> uint32_t;
  static auto GetCommandPool() -> VkCommandPool const&;
  static auto GetCommandBuffer(const uint32_t buffer_index) -> VkCommandBuffer const&;
  static auto GetSwapChain() -> VkSwapchainKHR const&;
  static auto GetImages() -> std::vector<VkImage> const&;
  static auto GetImageViews() -> std::vector<VkImageView> const&;
  static auto GetFormat() -> VkFormat const&;
  static auto GetExtent() -> VkExtent2D const&;
  static auto GetRenderPass() -> VkRenderPass const&;
  static auto GetFramebuffer(const uint32_t index) -> VkFramebuffer const&;

  static void RecordCommandBuffers(const int image_index);
  static void ResetCommandBuffer(const uint32_t buffer_index,
                                 const VkCommandBufferResetFlagBits flags = static_cast<VkCommandBufferResetFlagBits>(0));
  static void DrawFrame(Driver* driver, const Tick& current, const Tick& previous);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
