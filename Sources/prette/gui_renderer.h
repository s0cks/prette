#ifndef PRT_GUI_RENDERER_H
#define PRT_GUI_RENDERER_H

#include <vulkan/vulkan_core.h>

#include "prette/event.h"
#include "prette/gfx.h"

namespace prt {
#define FOR_EACH_GUI_RENDERER_EVENT(V) V(GuiRendererInit)

class GuiRenderer;
class GuiRendererEvent;

#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_GUI_RENDERER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class GuiRendererEvent : public Event {
 protected:
  GuiRendererEvent() = default;

 public:
  ~GuiRendererEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(GuiRenderer, FOR_EACH_GUI_RENDERER_EVENT);
};

class GuiRendererInitEvent : public GuiRendererEvent {
 public:
  GuiRendererInitEvent();
  ~GuiRendererInitEvent() override = default;
  DECLARE_EVENT_TYPE(GuiRendererEvent, GuiRendererInit);
};

DEFINE_EVENT_SUBJECT(GuiRenderer);
DEFINE_EVENT_OBSERVABLE(GuiRenderer);
FOR_EACH_GUI_RENDERER_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnGuiRendererEvent() -> GuiRendererEventObservable;
#define DEFINE_ON_EVENT(Name)                                                       \
  static inline auto On##Name##Event()->Name##EventObservable {                     \
    return OnGuiRendererEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_GUI_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class Driver;
class GuiRenderer {
 private:
  static void InitPipeline(const Driver* driver);
  static void InitFramebuffers(const Driver* driver);
  static void InitRenderPass(const Driver* driver);
  static void InitCommandPool(const Driver* driver);
  static void InitCommandBuffers(const Driver* driver);
  static void InitDescriptorPool(const Driver* driver);
  static void InitSceneDescriptors(const Driver* driver);
  static void InitTextureSampler(const Driver* driver);

  static void Destroy(const Driver* driver, const bool is_reinit);

 public:
  static void Init();
  static auto GetPass() -> VkRenderPass const&;
  static auto GetSceneDescriptor(const uint32_t frame) -> VkDescriptorSet const&;
  static auto GetDescriptorPool() -> VkDescriptorPool const&;
  static void Draw(const uint32_t bidx, const uint32_t image_index, std::vector<VkCommandBuffer>& cmd_buffers);
};
}  // namespace prt

#endif  // PRT_GUI_RENDERER_H
