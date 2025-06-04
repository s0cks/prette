#ifndef PRT_GUI_RENDERER_H
#define PRT_GUI_RENDERER_H

#include <vector>
#include <vulkan/vulkan_core.h>

#include "prette/event.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/gfx.h"
#include "prette/render_pass/render_pass.h"
#include "prette/rx.h"
#include "prette/vk.h"

namespace prt {
#define FOR_EACH_GUI_RENDERER_EVENT(V) V(GuiRenderPassInit)

class GuiRenderPass;
class GuiRenderPassEvent;

#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_GUI_RENDERER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(GuiRenderPass, FOR_EACH_GUI_RENDERER_EVENT);

class GuiRenderPassInitEvent : public GuiRenderPassEvent {
 public:
  GuiRenderPassInitEvent();
  ~GuiRenderPassInitEvent() override = default;
  DECLARE_EVENT_TYPE(GuiRenderPassEvent, GuiRenderPassInit);
};

DEFINE_EVENT_SUBJECT(GuiRenderPass);
DEFINE_EVENT_OBSERVABLE(GuiRenderPass);
FOR_EACH_GUI_RENDERER_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnGuiRenderPassEvent() -> GuiRenderPassEventObservable;
#define DEFINE_ON_EVENT(Name)                                                         \
  static inline auto On##Name##Event()->Name##EventObservable {                       \
    return OnGuiRenderPassEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_GUI_RENDERER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

namespace vk {
class RenderPassBuilder;
}

class Driver;
class GuiRenderPass : public vk::RenderPass {
  friend class vk::RenderPassBuilder;

 private:
  std::vector<Framebuffer*> framebuffers_{};
  rx::subscription on_tick_{};

  GuiRenderPass(const VkRenderPassCreateInfo* create_info);

  void InitImgui();
  void InitFramebuffers();

 protected:
  void OnSwapInit(const bool reinit) override;
  void OnSwapDestroyed(const bool reinit) override;

 public:
  ~GuiRenderPass() override;

  auto GetCurrentSceneDescriptor() const -> VkDescriptorSet const&;
  void Execute() override;

 public:
  static auto New() -> GuiRenderPass*;
};
}  // namespace prt

#endif  // PRT_GUI_RENDERER_H
