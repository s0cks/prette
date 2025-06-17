#ifndef PRT_EVENT_H
#define PRT_EVENT_H

#include <string>

#include "prette/assertions.h"  // IWYU pragma: keep
#include "prette/common.h"
#include "prette/rx.h"

namespace prt {
class Event {
  DEFINE_NON_COPYABLE_TYPE(Event);

 protected:
  Event() = default;

 public:
  virtual ~Event() = default;
  virtual auto GetName() const -> const char* = 0;
  virtual auto ToString() const -> std::string = 0;
#ifdef PRT_ENABLE_LUA
  virtual void ToTable(lua_State* L) const;
#endif  // PRT_ENABLE_LUA
};

#define DEFINE_EVENT_PROTOTYPE_TYPE_CHECK(Name)  \
  virtual auto As##Name##Event()->Name##Event* { \
    return nullptr;                              \
  }                                              \
  auto Is##Name##Event()->bool {                 \
    return As##Name##Event() != nullptr;         \
  }

#define DEFINE_EVENT_PROTOTYPE_TYPE(Name, Types)       \
 public:                                               \
  using Predicate = std::function<bool(Name##Event*)>; \
                                                       \
 public:                                               \
  Types(DEFINE_EVENT_PROTOTYPE_TYPE_CHECK)

#define DEFINE_EVENT_PROTOTYPE(Name, Types)   \
  class Name##Event : public Event {          \
   protected:                                 \
    Name##Event() = default;                  \
                                              \
   public:                                    \
    ~Name##Event() override = default;        \
    DEFINE_EVENT_PROTOTYPE_TYPE(Name, Types); \
  };

#define DECLARE_EVENT_TYPE(Proto, Name)                   \
  DEFINE_NON_COPYABLE_TYPE(Name##Event);                  \
                                                          \
 public:                                                  \
  using ParentEventType = Proto;                          \
                                                          \
 public:                                                  \
  auto ToString() const -> std::string override;          \
  auto GetName() const -> const char* override {          \
    return #Name;                                         \
  }                                                       \
  auto As##Name##Event()->Name##Event* override {         \
    return this;                                          \
  }                                                       \
  static inline auto Filter(Proto* event) -> bool {       \
    return event && event->Is##Name##Event();             \
  }                                                       \
  static inline auto Cast(Proto* event) -> Name##Event* { \
    ASSERT(event);                                        \
    ASSERT(event->Is##Name##Event());                     \
    return event->As##Name##Event();                      \
  }

#define __DECLARE_STATE_EVENT_STATE_CHECK(Name) \
  inline auto Is##Name() const->bool {          \
    return IsState(StateType::k##Name);         \
  }

#define __DECLARE_STATE_EVENT_STATE_FILTER(Name)                        \
  static inline auto FilterBy##Name##State()->ParentEventType::Filter { \
    return FilterByState(StateType::k##Name);                           \
  }

#define DECLARE_STATE_EVENT_TYPE(Proto, EventStateType, ForEachState)                                         \
  DECLARE_EVENT_TYPE(Proto, EventStateType);                                                                  \
                                                                                                              \
 public:                                                                                                      \
  using StateType = EventStateType;                                                                           \
                                                                                                              \
 public:                                                                                                      \
  auto GetState() const -> StateType;                                                                         \
  inline auto IsState(const StateType rhs) const -> bool {                                                    \
    return GetState() == rhs;                                                                                 \
  }                                                                                                           \
  ForEachState(__DECLARE_STATE_EVENT_STATE_CHECK);                                                            \
                                                                                                              \
 public:                                                                                                      \
  static inline auto FilterByState(const StateType rhs) -> ParentEventType::Filter {                          \
    return [rhs](ParentEventType* event) {                                                                    \
      return event && event->Is##EventStateType##Event() && event->As##EventStateType##Event()->IsState(rhs); \
    };                                                                                                        \
  }                                                                                                           \
  ForEachState(__DECLARE_STATE_EVENT_STATE_FILTER);

template <class E>
class EventSource {
  DEFINE_NON_COPYABLE_TYPE(EventSource<E>);

 protected:
  EventSource() = default;
  virtual void PublishEvent(E* event) const = 0;

  template <class T, typename... Args>
  void Publish(Args... args) const {
    T event(args...);
    return PublishEvent(&event);
  }

 public:
  virtual ~EventSource() = default;
  virtual auto OnEvent() const -> rx::observable<E*> = 0;
};

class Topic {
 protected:
  Topic() = default;

  template <class E>
  void PublishEventTo(const rx::subject<E*>& subject, E* event) const {
    ASSERT(event);
    const auto& subscriber = subject.get_subscriber();
    return subscriber.on_next(event);
  }

 public:
  ~Topic() = default;
};

template <class E>
class EventSourceTemplate : public EventSource<E> {
 protected:
  rx::subject<E*> events_{};  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

  EventSourceTemplate() = default;

  void PublishEvent(E* event) const override {
    ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    return subscriber.on_next(event);
  }

 public:
  ~EventSourceTemplate() override = default;

  auto OnEvent() const -> rx::observable<E*> override {
    return events_.get_observable();
  }
};

#define DEFINE_ON_GLOBAL_EVENT(Subject, Name)                                \
  static inline auto On##Name##Event()->Name##EventObservable {              \
    return On##Subject().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                          \
  template <class... ArgN>                                                   \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {    \
    return On##Name##Event().subscribe(args...);                             \
  }

// TODO:refactor On##Name()
#define DECLARE_GLOBAL_EVENT_SUBJECT(Name, ForEachEvent)                  \
  auto On##Name##Event()->Name##EventObservable;                          \
  template <typename... ArgN>                                             \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription { \
    return On##Name##Event().subscribe(args...);                          \
  }                                                                       \
  ForEachEvent(DEFINE_ON_GLOBAL_EVENT);

#define DEFINE_GLOBAL_EVENT_SUBJECT(Name, SubjectName) \
  static Name##Subject SubjectName{};                  \
  auto Get##Name##Observable()->Name##Observable {     \
    return (SubjectName).get_observable();             \
  }

#define DEFINE_EVENT_SUBJECT(Name)    using Name##EventSubject = rx::subject<Name##Event*>;

#define DEFINE_EVENT_OBSERVABLE(Name) using Name##EventObservable = rx::observable<Name##Event*>;
}  // namespace prt

#endif  // PRT_EVENT_H
