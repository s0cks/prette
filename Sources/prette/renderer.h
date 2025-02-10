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

class Renderer {
  friend class LuaState;

 private:
  static void InitSyncObjects(Driver* driver);
  static void InitResizeListener();
  static void DestroySyncObjects(Driver* driver);

 private:
  static void InitLua(lua_State* L);

 public:
  static void Init(Driver* driver);
  static void Shutdown(Driver* driver);
  static void DrawFrame(Driver* driver, const Tick& current, const Tick& previous);
};
}  // namespace prt

#endif  // PRT_RENDERER_H
