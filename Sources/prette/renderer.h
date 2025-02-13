#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vulkan/vulkan_core.h>

#include <vector>

#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/tick.h"

namespace prt {
#define FOR_EACH_RENDERER_EVENT(V) \
  V(SwapChainInit)                 \
  V(RendererInit)                  \
  V(PreFrame)                      \
  V(PostFrame)                     \
  V(SwapChainDestroyed)            \
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

class RendererEventBase : public TemplateRendererEvent<false> {
 protected:
  RendererEventBase() = default;

 public:
  ~RendererEventBase() override = default;
};

class FrameEvent : public TemplateRendererEvent<true> {
 protected:
  FrameEvent() = default;

 public:
  ~FrameEvent() override = default;
};

class SwapChainEvent : public RendererEventBase {
 private:
  bool reinit_;

 public:
  explicit SwapChainEvent(const bool reinit) :
    RendererEventBase(),
    reinit_(reinit) {}
  ~SwapChainEvent() override = default;

  auto IsReinit() const -> bool {
    return reinit_;
  }
};

class SwapChainInitEvent : public SwapChainEvent {
 public:
  explicit SwapChainInitEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainInitEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, SwapChainInit);
};

class RendererInitEvent : public RendererEventBase {
 public:
  RendererInitEvent() = default;
  ~RendererInitEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, RendererInit);
};

class PreFrameEvent : public FrameEvent {
 public:
  PreFrameEvent() = default;
  ~PreFrameEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, PreFrame);
};

class PostFrameEvent : public FrameEvent {
 public:
  PostFrameEvent() = default;
  ~PostFrameEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, PostFrame);
};

class SwapChainDestroyedEvent : public SwapChainEvent {
 public:
  explicit SwapChainDestroyedEvent(const bool reinit) :
    SwapChainEvent(reinit) {}
  ~SwapChainDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(RendererEvent, SwapChainDestroyed);
};

class RendererDestroyedEvent : public RendererEventBase {
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
  static void InitResizeListener();
  static void InitCommandPool(const Driver* driver);
  static void InitSwapChain(const Driver* driver, const bool reinit);
  static void ReInitSwapChain(Driver* driver);
  static void Destroy();
  static void DestroySwapChain(Driver* driver, const bool reinit);
  static auto AcquireNextImage(Driver* driver, uint32_t* result) -> bool;
  static void Submit(Driver* driver, const VkSemaphore& signal, const std::vector<VkCommandBuffer>& cmd_buffers);
  static void Present(Driver* driver, const VkSemaphore& signal, const uint32_t image_index);

 private:
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static auto GetCurrentFrame() -> uint32_t;
  static auto GetCommandPool() -> VkCommandPool const&;
  static auto GetSwapChain() -> VkSwapchainKHR const&;
  static auto GetImages() -> std::vector<VkImage> const&;
  static auto GetImageViews() -> std::vector<VkImageView> const&;
  static auto GetImageView(const uint32_t idx) -> VkImageView const&;
  static auto GetFormat() -> VkFormat const&;
  static auto GetExtent() -> VkExtent2D const&;
  static void DrawFrame(Driver* driver, const Tick& current, const Tick& previous);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
