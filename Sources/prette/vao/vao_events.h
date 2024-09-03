#ifndef PRT_VAO_EVENTS_H
#define PRT_VAO_EVENTS_H

#include <functional>
#include "prette/event.h"
#include "prette/vao/vao_id.h"

namespace prt::vao {
#define FOR_EACH_VAO_EVENT(V) \
  V(VaoCreated)               \
  V(VaoDestroyed)

  class Vao;
  class VaoEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_VAO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  typedef std::function<bool(VaoEvent*)> VaoEventPredicate;

  class VaoEvent : public Event {
  protected:
    const Vao* vao_;

    explicit VaoEvent(const Vao* vao):
      Event(),
      vao_(vao) {
      PRT_ASSERT(vao);
    }
  public:
    ~VaoEvent() override = default;

    const Vao* GetVao() const {
      return vao_;
    }

    VaoId GetVaoId() const;
#define DEFINE_TYPE_CHECK(Name)                                       \
    virtual Name##Event* As##Name##Event() { return nullptr; }        \
    bool Is##Name##Event() { return As##Name##Event() != nullptr; }
    FOR_EACH_VAO_EVENT(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  public:
    static inline VaoEventPredicate
    FilterBy(const VaoId id) {
      return [id](VaoEvent* event) {
        return event
            && event->GetVaoId() == id;
      };
    }
  };

#define DECLARE_VAO_EVENT(Name)                                 \
  public:                                                       \
    std::string ToString() const override;                      \
    const char* GetName() const override { return #Name; }      \
    Name##Event* As##Name##Event() override { return this; }    \
    static inline bool                                          \
    Filter(VaoEvent* event) {                                   \
      return event                                              \
          && event->Is##Name##Event();                          \
    }                                                           \
    static inline VaoEventPredicate                             \
    FilterBy(const VaoId id) {                                  \
      return [id](VaoEvent* event) {                            \
        return event                                            \
            && event->Is##Name##Event()                         \
            && event->GetVaoId() == id;                         \
      };                                                        \
    }                                                           \
    static inline Name##Event*                                  \
    Cast(VaoEvent* event) {                                     \
      PRT_ASSERT(event);                                        \
      PRT_ASSERT(event->Is##Name##Event());                     \
      return event->As##Name##Event();                          \
    }

  class VaoCreatedEvent : public VaoEvent {
  public:
    explicit VaoCreatedEvent(const Vao* vao):
      VaoEvent(vao) {
    }
    ~VaoCreatedEvent() override = default;
    DECLARE_VAO_EVENT(VaoCreated);
  };

  class VaoDestroyedEvent : public VaoEvent {
  public:
    explicit VaoDestroyedEvent(const Vao* vao):
      VaoEvent(vao) {
    }
    ~VaoDestroyedEvent() override = default;
    DECLARE_VAO_EVENT(VaoDestroyed);
  };

  typedef rx::subject<VaoEvent*> VaoEventSubject;

  class VaoEventSource {
  protected:
    VaoEventSource() = default;
  public:
    virtual ~VaoEventSource() = default;
    virtual rx::observable<VaoEvent*> OnEvent() const = 0;

#define DEFINE_ON_VAO_EVENT(Name)   \
    inline rx::observable<Name##Event*> On##Name##Event() const {   \
      return OnEvent()                                              \
        .filter(Name##Event::Filter)                                \
        .map(Name##Event::Cast);                                    \
    }
    FOR_EACH_VAO_EVENT(DEFINE_ON_VAO_EVENT);
#undef DEFINE_ON_VAO_EVENT
  };
}

#endif //PRT_VAO_EVENTS_H