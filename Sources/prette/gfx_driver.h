#ifndef PRT_GFX_DRIVER_H
#define PRT_GFX_DRIVER_H

#include "prette/gfx_driver_event.h"

namespace prt {
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

class LuaState;
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
