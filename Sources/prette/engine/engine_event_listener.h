#ifndef PRT_ENGINE_EVENT_LISTENER_H
#define PRT_ENGINE_EVENT_LISTENER_H

#include "prette/engine/engine.h"

namespace prt::engine {
  template<class E>
  class EventListener {
  private:
    rx::subscription sub_;
  protected:
    EventListener(Engine* engine, std::function<void(E*)> on_next):
      sub_() {
      PRT_ASSERT(engine);
      sub_ = engine->OnEvent()
        .filter(E::Filter)
        .map(E::Cast)
        .subscribe(on_next);
    }
  public:
    virtual ~EventListener() {
      if(sub_.is_subscribed())
        sub_.unsubscribe();
    }
  };

  class PreTickEventListener : public EventListener<PreTickEvent> {
  protected:
    explicit PreTickEventListener(Engine* engine):
      EventListener(engine, [this](PreTickEvent* event) { return OnPreTick(event); }) {
    }

    virtual void OnPreTick(PreTickEvent* event) = 0;
  public:
    ~PreTickEventListener() override = default;
  };
}

#endif //PRT_ENGINE_EVENT_LISTENER_H