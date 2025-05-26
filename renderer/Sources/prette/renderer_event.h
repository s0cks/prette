#ifndef PRT_RENDERER_EVENT_H
#define PRT_RENDERER_EVENT_H

#include <functional>

#include "prette/event.h"
#include "prette/renderer_state.h"

namespace prt {
#define FOR_EACH_RENDERER_EVENT(V) \
  FOR_EACH_RENDERER_STATE(V)       \
  V(InitDescriptorSets)            \
  V(InitPipelineLayouts)           \
  V(InitGraphicsPipelines)         \
  V(InitRenderPasses)              \
  V(InitBuffers)                   \
  V(PreFrame)                      \
  V(PostFrame)

class Renderer;
class RendererEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_RENDERER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class FrameEvent;
class RendererEvent : public Event {
 public:
  using Filter = std::function<bool(RendererEvent*)>;

 public:
  RendererEvent() = default;
  ~RendererEvent() override = default;

  virtual auto AsFrameEvent() -> FrameEvent* {
    return nullptr;
  }

  inline auto IsFrameEvent() -> bool {
    return AsFrameEvent() != nullptr;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Renderer, FOR_EACH_RENDERER_EVENT);
};

#define DECLARE_RENDERER_EVENT(Name)         \
  class Name##Event : public RendererEvent { \
   public:                                   \
    Name##Event() = default;                 \
    ~Name##Event() override = default;       \
    DECLARE_EVENT_TYPE(RendererEvent, Name); \
  };
DECLARE_RENDERER_EVENT(RendererCreated);
DECLARE_RENDERER_EVENT(InitRenderPasses);
DECLARE_RENDERER_EVENT(InitDescriptorSets);
DECLARE_RENDERER_EVENT(InitPipelineLayouts);
DECLARE_RENDERER_EVENT(InitGraphicsPipelines);
DECLARE_RENDERER_EVENT(InitBuffers);
DECLARE_RENDERER_EVENT(RendererDestroyed);
#undef DECLARE_RENDERER_EVENT

class FrameEvent : public RendererEvent {
 protected:
  FrameEvent() = default;

 public:
  ~FrameEvent() override = default;

  auto AsFrameEvent() -> FrameEvent* override {
    return this;
  }
};

#define DECLARE_FRAME_EVENT(Name)                   \
  class Name##FrameEvent : public FrameEvent {      \
   public:                                          \
    Name##FrameEvent() = default;                   \
    ~Name##FrameEvent() override = default;         \
    DECLARE_EVENT_TYPE(RendererEvent, Name##Frame); \
  };
DECLARE_FRAME_EVENT(Pre);
DECLARE_FRAME_EVENT(Post);
#undef DECLARE_FRAME_EVENT

DEFINE_EVENT_SUBJECT(Renderer);
DEFINE_EVENT_OBSERVABLE(Renderer);
FOR_EACH_RENDERER_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_RENDERER_EVENT_H
