#ifndef PRT_COMPONENT_EVENTS_H
#define PRT_COMPONENT_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::component {
#define FOR_EACH_COMPONENT_EVENT(V) \
  V(ComponentRegistered)            \
  V(ComponentStateCreated)          \
  V(ComponentStateDestroyed)

  class ComponentEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_COMPONENT_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class Component;
  class ComponentEvent : public Event {
  protected:
    Component* component_;

    explicit ComponentEvent(Component* component):
      Event(),
      component_(component) {
    }
  public:
    ~ComponentEvent() override = default;

    Component* component() const {
      return component_;
    }

    virtual bool IsComponentStateEvent() const {
      return false;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_COMPONENT_EVENT);
  };

#define DEFINE_COMPONENT_EVENT(Name)          \
  DECLARE_EVENT_TYPE(ComponentEvent, Name)

  class ComponentRegisteredEvent : public ComponentEvent {
  public:
    explicit ComponentRegisteredEvent(Component* component):
      ComponentEvent(component) {
    }
    ~ComponentRegisteredEvent() override = default;
    DEFINE_COMPONENT_EVENT(ComponentRegistered);
  };

  class ComponentStateEvent : public ComponentEvent {
  protected:
    explicit ComponentStateEvent(Component* component):
      ComponentEvent(component) {
    }
  public:
    ~ComponentStateEvent() override = default;

    bool IsComponentStateEvent() const override {
      return true;
    }
  };

  class ComponentStateCreatedEvent : public ComponentStateEvent {
  protected:
    explicit ComponentStateCreatedEvent(Component* component):
      ComponentStateEvent(component) {
    }
  public:
    ~ComponentStateCreatedEvent() override = default;
    DEFINE_COMPONENT_EVENT(ComponentStateCreated);
  };

  class ComponentStateDestroyedEvent : public ComponentStateEvent {
  protected:
    explicit ComponentStateDestroyedEvent(Component* component):
      ComponentStateEvent(component) {
    }
  public:
    ~ComponentStateDestroyedEvent() override = default;
    DEFINE_COMPONENT_EVENT(ComponentStateDestroyed);
  };
}

#undef DEFINE_COMPONENT_EVENT

#endif //PRT_COMPONENT_EVENTS_H