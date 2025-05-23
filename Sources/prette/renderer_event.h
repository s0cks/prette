#ifndef PRT_RENDERER_EVENT_H
#define PRT_RENDERER_EVENT_H

#include <functional>

#include "prette/event.h"
#include "prette/renderer_state.h"

namespace prt {
#define FOR_EACH_RENDERER_EVENT(V) \
  V(RendererState)                 \
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

class RendererStateEvent : public RendererEvent {
 private:
  RendererState state_;

 public:
  explicit RendererStateEvent(const RendererState state) :
    RendererEvent(),
    state_(state) {}
  ~RendererStateEvent() override = default;
  DECLARE_STATE_EVENT_TYPE(RendererEvent, RendererState, FOR_EACH_RENDERER_STATE);
};

class FrameEvent : public RendererEvent {
 protected:
  FrameEvent() = default;

 public:
  ~FrameEvent() override = default;

  auto AsFrameEvent() -> FrameEvent* override {
    return this;
  }
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

DEFINE_EVENT_SUBJECT(Renderer);
DEFINE_EVENT_OBSERVABLE(Renderer);
FOR_EACH_RENDERER_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_RENDERER_EVENT_H
