#ifndef PRT_GFX_DRIVER_H
#define PRT_GFX_DRIVER_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_DRIVER_EVENT(V) \
  V(DriverCreated)               \
  V(InstanceInit)                \
  V(PhysicalDeviceInit)          \
  V(DeviceInit)                  \
  V(SurfaceInit)                 \
  V(DriverInit)                  \
  V(DriverDestroyed)

class Driver;
class DriverEvent;
#define FORWARD_DECLARE_EVENT(Name) class Name##Event;
FOR_EACH_DRIVER_EVENT(FORWARD_DECLARE_EVENT)
#undef FORWARD_DECLARE_EVENT

class DriverEvent : public Event {
 protected:
  explicit DriverEvent() = default;

 public:
  ~DriverEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(Driver, FOR_EACH_DRIVER_EVENT);
};

class DriverEventBase : public DriverEvent {
 private:
  const Driver* driver_;

 public:
  explicit DriverEventBase(const Driver* driver) :
    DriverEvent(),
    driver_(driver) {
    ASSERT(driver_);
  }
  ~DriverEventBase() override = default;

  auto GetDriver() const -> const Driver* {
    return driver_;
  }
};

class DriverCreatedEvent : public DriverEventBase {
 public:
  explicit DriverCreatedEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~DriverCreatedEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, DriverCreated);
};

class InstanceInitEvent : public DriverEventBase {
 public:
  explicit InstanceInitEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~InstanceInitEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, InstanceInit);
};

class PhysicalDeviceInitEvent : public DriverEventBase {
 public:
  explicit PhysicalDeviceInitEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~PhysicalDeviceInitEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, PhysicalDeviceInit);
};

class SurfaceInitEvent : public DriverEventBase {
 public:
  explicit SurfaceInitEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~SurfaceInitEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, SurfaceInit);
};

class DeviceInitEvent : public DriverEventBase {
 public:
  explicit DeviceInitEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~DeviceInitEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, DeviceInit);
};

class DriverInitEvent : public DriverEventBase {
 public:
  explicit DriverInitEvent(const Driver* driver) :
    DriverEventBase(driver) {}
  ~DriverInitEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, DriverInit);
};

class DriverDestroyedEvent : public DriverEvent {
 public:
  DriverDestroyedEvent() = default;
  ~DriverDestroyedEvent() override = default;
  DECLARE_EVENT_TYPE(DriverEvent, DriverDestroyed);
};

DEFINE_EVENT_OBSERVABLE(Driver);
DEFINE_EVENT_SUBJECT(Driver);
FOR_EACH_DRIVER_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnDriverEvent() -> DriverEventObservable;

#define DEFINE_ON_DRIVER_EVENT(Name)                                           \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnDriverEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_DRIVER_EVENT(DEFINE_ON_DRIVER_EVENT);
#undef DEFINE_ON_DRIVER_EVENT

namespace engine {
class InitState;
}

class DriverBase {
  friend class LuaState;
  friend class engine::InitState;

 protected:
  static void PublishEvent(DriverEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

 protected:
  DriverBase() = default;

 public:
  virtual ~DriverBase() = default;

 private:
  static void DestroyDriver();
  static void InitDriver();
  static void InitLua(lua_State* L);

 public:
  static auto IsInitialized() -> bool;
  static void Init();
  static auto Get() -> Driver*;
};
}  // namespace prt

#endif  // PRT_GFX_DRIVER_H
