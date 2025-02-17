#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include <vulkan/vulkan_core.h>

#include <vector>

#include "prette/event.h"
#include "prette/gfx.h"
#include "prette/tick.h"

namespace prt {
#define FOR_EACH_RENDERER_EVENT(V) \
  V(RendererInit)                  \
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
  glm::vec3 pos;
  glm::vec3 color;

  static auto GetBindingDescription() -> VkVertexInputBindingDescription {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
  }

  static auto GetAttributeDescriptions() -> std::array<VkVertexInputAttributeDescription, 2> {
    std::array<VkVertexInputAttributeDescription, 2> attributes{};
    attributes.at(0).binding = 0;
    attributes.at(0).location = 0;
    attributes.at(0).format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes.at(0).offset = offsetof(Vertex, pos);

    attributes.at(1).binding = 0;
    attributes.at(1).location = 1;
    attributes.at(1).format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes.at(1).offset = offsetof(Vertex, color);
    return attributes;
  }
};
class Renderer {
  friend class LuaState;

 private:
  static void InitCommandBuffers(const Driver* driver);
  static void Destroy();

 private:
  static void InitLua(lua_State* L);

 public:
  static void Init();
  static void DrawFrame(Driver* driver, const Tick& current, const Tick& previous);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
