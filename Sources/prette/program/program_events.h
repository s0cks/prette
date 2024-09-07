#ifndef PRT_PROGRAM_EVENTS_H
#define PRT_PROGRAM_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"
#include "prette/shader/shader_id.h"
#include "prette/program/program_id.h"

namespace prt::program {
#define FOR_EACH_PROGRAM_EVENT(V) \
  V(ProgramCreated)               \
  V(ProgramShaderAttached)        \
  V(ProgramShaderDetached)        \
  V(ProgramLinked)                \
  V(ProgramDestroyed)

  class ProgramEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_PROGRAM_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  using ProgramEventPredicate = std::function<bool(ProgramEvent*)>;

  class ProgramEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(ProgramEvent);
  private:
    ProgramId id_;
  protected:
    explicit ProgramEvent(const ProgramId id):
      Event(),
      id_(id) {
    }
  public:
    ~ProgramEvent() override = default;

    auto GetProgramId() const -> ProgramId {
      return id_;
    }
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_PROGRAM_EVENT);
  };

#define DEFINE_PROGRAM_EVENT(Name)                          \
  DECLARE_EVENT_TYPE(ProgramEvent, Name)                    \
  static inline auto                                        \
  FilterBy(const ProgramId id) -> ProgramEventPredicate {   \
    return [id](ProgramEvent* event) {                      \
      return event                                          \
          && event->Is##Name##Event()                       \
          && event->GetProgramId() == id;                   \
    };                                                      \
  }

  class ProgramCreatedEvent : public ProgramEvent {
  public:
    explicit ProgramCreatedEvent(const ProgramId id):
      ProgramEvent(id) {
    }
    ~ProgramCreatedEvent() override = default;
    DEFINE_PROGRAM_EVENT(ProgramCreated);
  };

  class ProgramShaderEvent : public ProgramEvent {
    DEFINE_NON_COPYABLE_TYPE(ProgramShaderEvent);
  private:
    ShaderId shader_;
  protected:
    explicit ProgramShaderEvent(const ProgramId id, const ShaderId shader):
      ProgramEvent(id),
      shader_(shader) {
    }
  public:
    ~ProgramShaderEvent() override = default;

    auto GetShaderId() const -> ShaderId {
      return shader_;
    }
  };

  class ProgramShaderAttachedEvent : public ProgramShaderEvent {
  public:
    ProgramShaderAttachedEvent(const ProgramId id, const ShaderId shader):
      ProgramShaderEvent(id, shader) {
    }
    ~ProgramShaderAttachedEvent() override = default;
    DEFINE_PROGRAM_EVENT(ProgramShaderAttached);
  };

  class ProgramShaderDetachedEvent : public ProgramShaderEvent {
  public:
    ProgramShaderDetachedEvent(const ProgramId id, const ShaderId shader):
      ProgramShaderEvent(id, shader) {
    }
    ~ProgramShaderDetachedEvent() override = default;
    DEFINE_PROGRAM_EVENT(ProgramShaderDetached);
  };

  class ProgramLinkedEvent : public ProgramEvent {
  public:
    explicit ProgramLinkedEvent(const ProgramId id):
      ProgramEvent(id) {
    }
    ~ProgramLinkedEvent() override = default;
    DEFINE_PROGRAM_EVENT(ProgramLinked);
  };

  class ProgramDestroyedEvent : public ProgramEvent {
  public:
    explicit ProgramDestroyedEvent(const ProgramId id):
      ProgramEvent(id) {
    }
    ~ProgramDestroyedEvent() override = default;
    DEFINE_PROGRAM_EVENT(ProgramDestroyed);
  };

  using ProgramEventSubject=rx::subject<ProgramEvent *>;
  using ProgramEventObservable=rx::observable<ProgramEvent *>;
#define DEFINE_EVENT_OBSERVABLE(Name)                         \
  using Name##EventObservable=rx::observable<Name##Event*>;
  FOR_EACH_PROGRAM_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class ProgramEventSource : public EventSource<ProgramEvent> {
    DEFINE_NON_COPYABLE_TYPE(ProgramEventSource);
  protected:
    ProgramEventSource() = default;
  public:
    ~ProgramEventSource() override = default;
#define DEFINE_ON_EVENT(Name)                           \
    inline auto On##Name() -> Name##EventObservable {   \
      return OnEvent()                                  \
        .filter([](ProgramEvent* event) {               \
          return event                                  \
              && event->Is##Name##Event();              \
        })                                              \
        .map([](ProgramEvent* event) {                  \
          PRT_ASSERT(event);                            \
          PRT_ASSERT(event->Is##Name##Event());         \
          return event->As##Name##Event();              \
        });                                             \
    }
    FOR_EACH_PROGRAM_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };

#undef DEFINE_PROGRAM_EVENT
}

#endif //PRT_PROGRAM_EVENTS_H