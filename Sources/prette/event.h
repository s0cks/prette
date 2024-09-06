#ifndef PRT_EVENT_H
#define PRT_EVENT_H

#include <string>
#include "prette/rx.h"
#include "prette/common.h"

namespace prt {
  class Event {
  protected:
    Event() = default;
  public:
    virtual ~Event() = default;
    virtual const char* GetName() const = 0;
    virtual std::string ToString() const = 0;
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
  protected:
    EventSource() = default;
    virtual void Publish(E* event) = 0;

    template<class T, typename... Args>
    inline void Publish(Args... args) {
      T event(args...);
      return Publish((E*) &event);
    }
  public:
    virtual ~EventSource() = default;
    virtual rx::observable<E*> OnEvent() const = 0;
  };

  template<class E>
  class EventPublisher {
  private:
    rx::subject<E*> events_;
    rx::subscription fwd_;
  protected:
    EventPublisher() = default;
    EventPublisher(const rx::subject<E*>& fwd):
      events_(),
      fwd_() {
      ForwardTo(fwd);
    }

    inline void Publish(E* event) {
      PRT_ASSERT(event);
      const auto& subscriber = events_.get_subscriber();
      subscriber.on_next(event);
    }

    template<class T, typename... Args>
    inline void Publish(Args... args) {
      T event(args...);
      return Publish((E*) &event);
    }

    rx::subscriber<E*> GetSubscriber() const {
      return events_.get_subscriber();
    }

    inline void ForwardTo(const rx::subject<E*>& dst) {
      if(fwd_.is_subscribed())
        fwd_.unsubscribe();
      fwd_ = OnEvent().subscribe(dst.get_subscriber());
    }

    inline void ForwardTo(EventPublisher<E>* dst) {
      PRT_ASSERT(dst);
      return ForwardTo(dst->events_);
    }
  public:
    virtual ~EventPublisher() {
      if(fwd_.is_subscribed())
        fwd_.unsubscribe();
      const auto& subscriber = events_.get_subscriber();
      subscriber.on_completed();
    }

    rx::observable<E*> OnEvent() const {
      return events_.get_observable();
    }

    explicit operator rx::subject<E*>& () {
      return events_;
    }

    explicit operator const rx::subject<E*>& () const {
      return events_;
    }
  };
}

#endif //PRT_EVENT_H