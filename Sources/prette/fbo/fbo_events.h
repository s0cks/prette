#ifndef PRT_FRAMEBUFFER_EVENTS_H
#define PRT_FRAMEBUFFER_EVENTS_H

#include "prette/event.h"
#include "prette/fbo/fbo_id.h"

namespace prt::fbo {
#define FOR_EACH_FBO_EVENT(V) \
  V(FboCreated)               \
  V(FboDestroyed)

  class FboEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_FBO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class Fbo;
  class FboEvent : public Event {
  protected:
    const Fbo* fbo_;

    explicit FboEvent(const Fbo* fbo):
      Event(),
      fbo_(fbo) {
    }
  public:
    ~FboEvent() override = default;

    const Fbo* GetFbo() const {
      return fbo_;
    }

    FboId GetFboId() const;
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_FBO_EVENT);
  };

#define DECLARE_FRAMEBUFFER_EVENT(Name)                               \
  DECLARE_EVENT_TYPE(FboEvent, Name)                                  \
  static inline std::function<bool(FboEvent*)>                        \
  FilterBy(const FboId id) {                                          \
    return [id](FboEvent* event) {                                    \
      return event                                                    \
          && event->Is##Name##Event()                                 \
          && event->GetFboId() == id;                                 \
    };                                                                \
  }

  class FboCreatedEvent : public FboEvent {
  public:
    explicit FboCreatedEvent(const Fbo* fbo):
      FboEvent(fbo) {
    }
    ~FboCreatedEvent() override = default;
    DECLARE_FRAMEBUFFER_EVENT(FboCreated);
  };

  class FboDestroyedEvent : public FboEvent {
  public:
    explicit FboDestroyedEvent(const Fbo* fbo):
      FboEvent(fbo) {
    }
    ~FboDestroyedEvent() override = default;
    DECLARE_FRAMEBUFFER_EVENT(FboDestroyed);
  };

  typedef rx::subject<FboEvent*> FboEventSubject;
  typedef rx::observable<FboEvent*> FboEventObservable;
#define DEFINE_EVENT_OBSERVABLE(Name)             \
  typedef rx::observable<Name##Event*> Name##EventObservable;
  FOR_EACH_FBO_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class FboEventSource : public EventSource<FboEvent> {
  protected:
    FboEventSource() = default;
  public:
    ~FboEventSource() override = default;
#define DEFINE_ON_FBO_EVENT(Name)                     \
    inline Name##EventObservable On##Name() const {   \
      return OnEvent()                                \
        .filter([](FboEvent* event) {                 \
          return event                                \
              && event->Is##Name##Event();            \
        })                                            \
        .map([](FboEvent* event) {                    \
          PRT_ASSERT(event);                          \
          PRT_ASSERT(event->Is##Name##Event());       \
          return event->As##Name##Event();            \
        });                                           \
    }
    FOR_EACH_FBO_EVENT(DEFINE_ON_FBO_EVENT)
#undef DEFINE_ON_FBO_EVENT
  };
}

#endif //PRT_FRAMEBUFFER_EVENTS_H