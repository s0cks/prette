#ifndef PRT_DRIVER_EVENT_H
#define PRT_DRIVER_EVENT_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_DRIVER_EVENT(V) \
  V(DriverCreated)               \
  V(InstanceInit)                \
  V(PhysicalDeviceInit)          \
  V(DeviceInit)                  \
  V(SurfaceInit)                 \
  V(DriverInit)                  \
  V(DestroyingDriver)            \
  V(DriverDestroyed)

class Driver;
class DriverEvent;
#define FORWARD_DECLARE_EVENT(Name) class Name##Event;
FOR_EACH_DRIVER_EVENT(FORWARD_DECLARE_EVENT)
#undef FORWARD_DECLARE_EVENT

DEFINE_EVENT_PROTOTYPE(Driver, FOR_EACH_DRIVER_EVENT);

#define DECLARE_DRIVER_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(DriverEvent, Name);

#define DECLARE_DRIVER_EVENT(Name)         \
  class Name##Event : public DriverEvent { \
   public:                                 \
    Name##Event() = default;               \
    ~Name##Event() override = default;     \
    DECLARE_DRIVER_EVENT_TYPE(Name);       \
  };

DECLARE_DRIVER_EVENT(InstanceInit);
DECLARE_DRIVER_EVENT(PhysicalDeviceInit);
DECLARE_DRIVER_EVENT(DeviceInit);
DECLARE_DRIVER_EVENT(SurfaceInit);

DECLARE_DRIVER_EVENT(DriverCreated);
DECLARE_DRIVER_EVENT(DriverInit);
DECLARE_DRIVER_EVENT(DestroyingDriver);
DECLARE_DRIVER_EVENT(DriverDestroyed);

DEFINE_EVENT_OBSERVABLE(Driver);
DEFINE_EVENT_SUBJECT(Driver);
FOR_EACH_DRIVER_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_DRIVER_EVENT_H
