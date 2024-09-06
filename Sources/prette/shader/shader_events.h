#ifndef PRT_SHADER_EVENTS_H
#define PRT_SHADER_EVENTS_H

#include "prette/rx.h"
#include "prette/event.h"
#include "prette/resource/resource_event.h"
#include "prette/shader/shader_id.h"

namespace prt::shader {
#define FOR_EACH_SHADER_EVENT(V) \
  V(ShaderCreated)               \
  V(ShaderCompiled)              \
  V(ShaderDestroyed)

  class Shader;
  class ShaderEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_SHADER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class ShaderEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(ShaderEvent);
  private:
    const Shader* shader_;
  protected:
    explicit ShaderEvent(const Shader* shader):
      Event(),
      shader_(shader) {
      PRT_ASSERT(shader);
    }
  public:
    ~ShaderEvent() override = default;

    auto GetShader() const -> const Shader* {
      return shader_;
    }

    auto GetShaderId() const -> ShaderId;

    virtual auto IsCompilerEvent() const -> bool {
      return false;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_SHADER_EVENT);
  };

#define DECLARE_SHADER_EVENT(Name)                    \
  DECLARE_EVENT_TYPE(ShaderEvent, Name)

  class ShaderCreatedEvent : public resource::ResourceCreatedEvent,
                             public ShaderEvent {
  public:
    explicit ShaderCreatedEvent(const Shader* shader):
      ShaderEvent(shader),
      resource::ResourceCreatedEvent() {
    }
    ~ShaderCreatedEvent() override = default;
    DECLARE_SHADER_EVENT(ShaderCreated);
  };

  class ShaderDestroyedEvent : public resource::ResourceDestroyedEvent,
                               public ShaderEvent {
  public:
    explicit ShaderDestroyedEvent(const Shader* shader):
      ShaderEvent(shader),
      resource::ResourceDestroyedEvent() {
    }
    ~ShaderDestroyedEvent() override = default;
    DECLARE_SHADER_EVENT(ShaderDestroyed);
  };

  class ShaderCompilerEvent : public ShaderEvent {
    DEFINE_NON_COPYABLE_TYPE(ShaderCompilerEvent);
  protected:
    explicit ShaderCompilerEvent(const Shader* shader):
      ShaderEvent(shader) {
    }
  public:
    ~ShaderCompilerEvent() override = default;

    auto IsCompilerEvent() const -> bool override {
      return true;
    }
  };

  class ShaderCompiledEvent : public ShaderCompilerEvent {
  public:
    explicit ShaderCompiledEvent(const Shader* shader):
      ShaderCompilerEvent(shader) {
    }
    ~ShaderCompiledEvent() override = default;
    DECLARE_SHADER_EVENT(ShaderCompiled);
  };

  using ShaderEventSubject = rx::subject<ShaderEvent*>;
  using ShaderEventObservable = rx::observable<ShaderEvent*>;
#define DEFINE_SHADER_EVENT_OBSERVABLE(Name)                      \
  using Name##EventObservable = rx::observable<Name##Event*>;
  FOR_EACH_SHADER_EVENT(DEFINE_SHADER_EVENT_OBSERVABLE)
#undef DEFINE_SHADER_EVENT_OBSERVABLE

  class ShaderEventSource : public EventSource<ShaderEvent> {
    DEFINE_NON_COPYABLE_TYPE(ShaderEventSource);
  protected:
    ShaderEventSource() = default;
  public:
    ~ShaderEventSource() override = default;

#define DEFINE_ON_EVENT(Name)                     \
    inline auto                                   \
    On##Name() -> Name##EventObservable {         \
      return OnEvent()                            \
        .filter([](ShaderEvent* event) {          \
          return event                            \
              && event->Is##Name##Event();        \
        })                                        \
        .map([](ShaderEvent* event) {             \
          PRT_ASSERT(event);                      \
          PRT_ASSERT(event->Is##Name##Event());   \
          return event->As##Name##Event();        \
        });                                       \
    }
    FOR_EACH_SHADER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_SHADER_EVENTS_H