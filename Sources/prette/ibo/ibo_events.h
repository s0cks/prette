#ifndef PRT_IBO_EVENTS_H
#define PRT_IBO_EVENTS_H

#include "prette/event.h"
#include "prette/ibo/ibo_id.h"

namespace prt::ibo {
#define FOR_EACH_IBO_EVENT(V) \
  V(IboCreated)               \
  V(IboDestroyed)

#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_IBO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class Ibo;
  class IboEvent : public Event {
  protected:
    const Ibo* ibo_;

    explicit IboEvent(const Ibo* ibo):
      Event(),
      ibo_(ibo) {
      PRT_ASSERT(ibo);
    }
  public:
    ~IboEvent() override = default;

    const Ibo* GetIbo() const {
      return ibo_;
    }

    IboId GetIboId() const;
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_IBO_EVENT);
  };

#define DECLARE_IBO_EVENT(Name)                   \
  DECLARE_EVENT_TYPE(IboEvent, Name)              \
  static inline std::function<bool(IboEvent*)>    \
  FilterBy(const IboId id) {                      \
    return [id](IboEvent* event) {                \
      return event                                \
          && event->Is##Name##Event()             \
          && event->GetIboId() == id;             \
    };                                            \
  }

  class IboCreatedEvent : public IboEvent {
  public:
    explicit IboCreatedEvent(const Ibo* ibo):
      IboEvent(ibo) {
    }
    ~IboCreatedEvent() override = default;
    DECLARE_IBO_EVENT(IboCreated);
  };

  class IboDestroyedEvent : public IboEvent {
  public:
    explicit IboDestroyedEvent(const Ibo* ibo):
      IboEvent(ibo) {
    }
    ~IboDestroyedEvent() override = default;
    DECLARE_IBO_EVENT(IboDestroyed);
  };

  typedef rx::subject<IboEvent*> IboEventSubject;

  class IboEventSource {
  protected:
    IboEventSource() = default;
  public:
    virtual ~IboEventSource() = default;
    virtual rx::observable<IboEvent*> OnEvent() const = 0;
#define DEFINE_ON_IBO_EVENT(Name)                             \
    rx::observable<Name##Event*> On##Name##Event() const {    \
      return OnEvent()                                        \
        .filter(Name##Event::Filter)                          \
        .map(Name##Event::Cast);                              \
    }
    FOR_EACH_IBO_EVENT(DEFINE_ON_IBO_EVENT)
#undef DEFINE_ON_IBO_EVENT
  };
}

#endif //PRT_IBO_EVENTS_H