#ifndef PRT_RENDER_EVENTS_H
#define PRT_RENDER_EVENTS_H

#include "prette/rx.h"
#include "prette/event.h"
#include "prette/common.h"
#include "prette/uv/uv_tick.h"

namespace prt::render {
#define FOR_EACH_RENDER_EVENT(V) \
  V(RendererInitialized)         \
  V(PreRender)                   \
  V(PostRender)                  \
  V(RenderModeChanged)

  class RenderEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_RENDER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class RenderEvent : public Event {
  public:
    RenderEvent() = default;
    ~RenderEvent() override = default;

#define TYPE_CHECK(Name)                                              \
    virtual Name##Event* As##Name##Event() { return nullptr; }        \
    bool Is##Name##Event() { return As##Name##Event() != nullptr; }
    FOR_EACH_RENDER_EVENT(TYPE_CHECK)
#undef TYPE_CHECK
  };

#define DEFINE_RENDER_EVENT(Name) \
  public:                                                           \
    const char* GetName() const override { return #Name; }          \
    std::string ToString() const override;                          \
    Name##Event* As##Name##Event() override { return this; }         \
    static inline bool                                              \
    Filter(RenderEvent* event) {                                    \
      return event                                                  \
          && event->Is##Name##Event();                              \
    }                                                               \
    static inline Name##Event*                                      \
    Cast(RenderEvent* event) {                                      \
      PRT_ASSERT(event);                                            \
      PRT_ASSERT(event->Is##Name##Event());                         \
      return event->As##Name##Event();                              \
    }

  class Renderer;
  class RendererInitializedEvent : public RenderEvent {
  protected:
    const Renderer* renderer_;
  public:
    explicit RendererInitializedEvent(const Renderer* renderer):
      RenderEvent(),
      renderer_(renderer) {
    }
    ~RendererInitializedEvent() override = default;

    const Renderer* GetRenderer() const {
      return renderer_;
    }

    DEFINE_RENDER_EVENT(RendererInitialized);
  };

  class PreRenderEvent : public RenderEvent {
  public:
    PreRenderEvent() = default;
    ~PreRenderEvent() override = default;
    DEFINE_RENDER_EVENT(PreRender);
  };

  class PostRenderEvent : public RenderEvent {
  private:
    uv::Tick tick_;
  public:
    explicit PostRenderEvent(const uv::Tick& tick):
      RenderEvent(),
      tick_(tick) {  
    }
    ~PostRenderEvent() override = default;

    const uv::Tick& tick() const {
      return tick_;
    }

    DEFINE_RENDER_EVENT(PostRender);
  };
  
  class RenderModeChangedEvent : public RenderEvent {
  public:
    RenderModeChangedEvent() = default;
    ~RenderModeChangedEvent() override = default;
    DEFINE_RENDER_EVENT(RenderModeChanged);
  };

  typedef rx::subject<RenderEvent*> RenderEventSubject;

  class RenderEventPublisher : public EventPublisher<RenderEvent> {
  protected:
    RenderEventPublisher() = default;
  public:
    ~RenderEventPublisher() override = default;
  };
}

#endif //PRT_RENDER_EVENTS_H