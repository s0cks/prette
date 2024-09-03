#ifndef PRT_CAMERA_EVENTS_H
#define PRT_CAMERA_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::camera {
#define FOR_EACH_CAMERA_EVENT(V)    \
  V(CameraCreated)                  \
  V(CameraDestroyed)

  class Camera;
  class CameraEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_CAMERA_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class CameraEvent : public Event {
  protected:
    CameraEvent() = default;
  public:
    ~CameraEvent() override = default;
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_CAMERA_EVENT);
  };

#define DECLARE_CAMERA_EVENT(Name)          \
  DECLARE_EVENT_TYPE(CameraEvent, Name)

  class CameraCreatedEvent : public CameraEvent {
  public:
    CameraCreatedEvent() = default;
    ~CameraCreatedEvent() override = default;
    DECLARE_CAMERA_EVENT(CameraCreated);
  };
  
  class CameraDestroyedEvent : public CameraEvent {
  public:
    CameraDestroyedEvent() = default;
    ~CameraDestroyedEvent() override = default;
    DECLARE_CAMERA_EVENT(CameraDestroyed);
  };
}

#endif //PRT_CAMERA_EVENTS_H