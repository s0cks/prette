#ifndef PRT_COMPONENT_EVENT_H
#define PRT_COMPONENT_EVENT_H

#include <utility>

#include "prette/entity/entity.h"
#include "prette/event.h"

namespace prt {
class Component;
#define FOR_EACH_COMPONENT_EVENT(V) \
  V(RegisterComponents)             \
  V(ComponentAdded)                 \
  V(ComponentRemoved)

#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_COMPONENT_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Component, FOR_EACH_COMPONENT_EVENT);

#define DEFINE_COMPONENT_EVENT(Name)          \
  class Name##Event : public ComponentEvent { \
   public:                                    \
    Name##Event() = default;                  \
    ~Name##Event() override = default;        \
    DECLARE_EVENT_TYPE(ComponentEvent, Name); \
  };

DEFINE_COMPONENT_EVENT(RegisterComponents);

#undef DEFINE_COMPONENT_EVENT

class ComponentAddedEvent : public ComponentEvent {
 private:
  Entity entity_;
  Component* component_;

 public:
  ComponentAddedEvent(Entity entity, Component* component) :
    ComponentEvent(),
    entity_(std::move(entity)),
    component_(component) {}
  ~ComponentAddedEvent() override = default;
  DECLARE_EVENT_TYPE(ComponentEvent, ComponentAdded);
};

class ComponentRemovedEvent : public ComponentEvent {
 private:
  Entity entity_;
  Component* component_;

 public:
  ComponentRemovedEvent(Entity entity, Component* component) :
    ComponentEvent(),
    entity_(std::move(entity)),
    component_(component) {}
  ~ComponentRemovedEvent() override = default;
  DECLARE_EVENT_TYPE(ComponentEvent, ComponentRemoved);
};

DEFINE_EVENT_OBSERVABLE(Component);
DEFINE_EVENT_SUBJECT(Component);
FOR_EACH_COMPONENT_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_COMPONENT_EVENT_H
