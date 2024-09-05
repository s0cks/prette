#ifndef PRT_VBO_EVENTS_H
#define PRT_VBO_EVENTS_H

#include "prette/event.h"
#include "prette/vbo/vbo_id.h"

namespace prt::vbo {
#define FOR_EACH_VBO_EVENT(V)    \
  V(VboCreated)                  \
  V(VboDestroyed)

  class Vbo;
  class VboEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_VBO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class VboEvent : public Event {
  protected:
    const Vbo* vbo_;

    explicit VboEvent(const Vbo* vbo):
      Event(),
      vbo_(vbo) {
    }
  public:
    ~VboEvent() override = default;

    const Vbo* GetVbo() const {
      return vbo_;
    }

    VboId GetVboId() const;
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_VBO_EVENT);
  };

#define DECLARE_VBO_EVENT(Name)                                     \
    DECLARE_EVENT_TYPE(VboEvent, Name)                              \
    static inline std::function<bool(VboEvent*)>                    \
    FilterBy(const VboId id) {                                      \
      return [id](VboEvent* event) {                                \
        return event                                                \
            && event->Is##Name##Event()                             \
            && event->GetVboId() == id;                             \
      };                                                            \
    }

  class VboCreatedEvent : public VboEvent {
  public:
    explicit VboCreatedEvent(const Vbo* vbo):
      VboEvent(vbo) {
    }
    ~VboCreatedEvent() override = default;
    DECLARE_VBO_EVENT(VboCreated);
  };

  class VboDestroyedEvent : public VboEvent {
  public:
    explicit VboDestroyedEvent(const Vbo* vbo):
      VboEvent(vbo) {
    }
    ~VboDestroyedEvent() override = default;
    DECLARE_VBO_EVENT(VboDestroyed);
  };

  typedef rx::subject<VboEvent*> VboEventSubject;
  typedef rx::observable<VboEvent*> VboEventObservable;
#define DEFINE_EVENT_OBSERVABLE(Name)                           \
  typedef rx::observable<Name##Event*> Name##EventObservable;
  FOR_EACH_VBO_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class VboEventSource {
  protected:
    VboEventSource() = default;
    virtual void Publish(VboEvent* event) = 0;

    template<class E, typename... Args>
    inline void Publish(Args... args) {
      E event(args...);
      return Publish((VboEvent*) &event);
    }
  public:
    virtual ~VboEventSource() = default;
    virtual VboEventObservable OnEvent() const = 0;
#define DEFINE_ON_VBO_EVENT(Name)                                 \
    virtual Name##EventObservable On##Name() const = 0;
    FOR_EACH_VBO_EVENT(DEFINE_ON_VBO_EVENT)
#undef DEFINE_ON_VBO_EVENT
  };
}

#endif //PRT_VBO_EVENTS_H