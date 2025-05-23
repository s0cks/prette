#ifndef PRT_GUI_RENDERER_H
#define PRT_GUI_RENDERER_H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/event.h"
#include "prette/framebuffer.h"
#include "prette/gfx.h"
#include "prette/render_pass.h"
#include "prette/rx.h"
#include "prette/sampler.h"
#include "prette/vk.h"

namespace prt {
#define FOR_EACH_GUI_RENDERER_EVENT(V) V(GuiRendererInit)

class GuiRenderer;
class GuiRendererEvent;

#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_GUI_RENDERER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(GuiRenderer, FOR_EACH_GUI_RENDERER_EVENT);

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

namespace vk {
class RenderPassBuilder;
}

class Driver;
class GuiRenderer : public vk::RenderPass {
  friend class vk::RenderPassBuilder;
  static auto CreateSampler() -> vk::Sampler*;

 private:
  vk::Sampler* sampler_ = nullptr;
  std::vector<vk::Framebuffer*> framebuffers_{};
  std::vector<VkDescriptorSet> descriptors_{};
  rx::subscription on_tick_{};

  GuiRenderer(const VkRenderPassCreateInfo* create_info);

  void InitImgui();
  void InitFramebuffers();

 protected:
  void OnSwapInit(const bool reinit) override;
  void OnSwapDestroyed(const bool reinit) override;

 public:
  ~GuiRenderer() override;

  auto GetSampler() const -> vk::Sampler* {
    return sampler_;
  }

  auto GetSceneDescriptor(const uint32_t frame) const -> VkDescriptorSet const& {
    return descriptors_[frame];
  }

  auto GetCurrentSceneDescriptor() const -> VkDescriptorSet const&;
  void Execute() override;

 public:
  static auto New() -> GuiRenderer*;
};

void InitGuiRenderer();
auto IsGuiRendererInitialized() -> bool;
auto GetGuiRenderer() -> GuiRenderer*;
}  // namespace prt

#endif  // PRT_GUI_RENDERER_H
