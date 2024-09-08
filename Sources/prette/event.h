#ifndef PRT_EVENT_H
#define PRT_EVENT_H

#include <string>
#include "prette/rx.h"
#include "prette/common.h"

namespace prt {
  class Event {
    DEFINE_NON_COPYABLE_TYPE(Event);
  protected:
    Event() = default;
  public:
    virtual ~Event() = default;
    virtual auto GetName() const -> const char* = 0;
    virtual auto ToString() const -> std::string = 0;
  };

#define DEFINE_EVENT_PROTOTYPE_TYPE_CHECK(Name)                             \
  virtual auto As##Name##Event() -> Name##Event* { return nullptr; }        \
  auto Is##Name##Event() -> bool { return As##Name##Event() != nullptr; }

#define DEFINE_EVENT_PROTOTYPE(Types)                                     \
  Types(DEFINE_EVENT_PROTOTYPE_TYPE_CHECK)

#define DECLARE_EVENT_TYPE(Proto, Name)                                   \
    DEFINE_NON_COPYABLE_TYPE(Name##Event);                                \
  public:                                                                 \
    auto ToString() const -> std::string override;                        \
    auto GetName() const -> const char* override { return #Name; }        \
    auto As##Name##Event() -> Name##Event* override { return this; }      \
    static inline auto                                                    \
    Filter(Proto* event) -> bool {                                        \
      return event                                                        \
          && event->Is##Name##Event();                                    \
    }                                                                     \
    static inline auto                                                    \
    Cast(Proto* event) -> Name##Event* {                                  \
      PRT_ASSERT(event);                                                  \
      PRT_ASSERT(event->Is##Name##Event());                               \
      return event->As##Name##Event();                                    \
    }

  template<class E>
  class EventSource {
    DEFINE_NON_COPYABLE_TYPE(EventSource<E>);
  protected:
    EventSource() = default;
    virtual void PublishEvent(E* event) = 0;

    template<class T, typename... Args>
    void Publish(Args... args) {
      T event(args...);
      return PublishEvent((E*) &event);
    }
  public:
    virtual ~EventSource() = default;
    virtual auto OnEvent() const -> rx::observable<E*> = 0;
  };

#define DEFINE_EVENT_SUBJECT(Name)                        \
  using Name##EventSubject = rx::subject<Name##Event*>;

#define DEFINE_EVENT_OBSERVABLE(Name)                           \
  using Name##EventObservable = rx::observable<Name##Event*>;
}

#endif //PRT_EVENT_H