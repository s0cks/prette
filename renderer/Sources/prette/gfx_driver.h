#ifndef PRT_GFX_DRIVER_H
#define PRT_GFX_DRIVER_H

#include "prette/gfx_driver_event.h"

#ifdef PRT_ENABLE_LUA
struct lua_State;
#endif  // PRT_ENABLE_LUA

namespace prt {
auto OnDriverEvent() -> DriverEventObservable;

#define DEFINE_ON_DRIVER_EVENT(Name)                                           \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnDriverEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                            \
  template <class... ArgN>                                                     \
  static inline auto On##Name(ArgN... args)->rx::composite_subscription {      \
    return On##Name##Event().subscribe(args...);                               \
  }
FOR_EACH_DRIVER_EVENT(DEFINE_ON_DRIVER_EVENT);
#undef DEFINE_ON_DRIVER_EVENT

namespace engine {
class InitState;
}

class DriverBase {
  friend class DriverFinalizer;
  friend class DriverInitializer;
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
  static void InitDriver();
#ifdef PRT_ENABLE_LUA
  static void InitLua(lua_State* L);
#endif  // PRT_ENABLE_LUA
 public:
  static void Init();
  static auto Get() -> Driver*;
  static auto IsInitialized() -> bool;
};

class DriverInitializer {
 public:
  DriverInitializer() = default;
  virtual ~DriverInitializer() = default;
  void InitDriver();

 public:
  static inline void Init() {
    DriverInitializer initializer{};
    return initializer.InitDriver();
  }
};

class DriverFinalizer {
 public:
  DriverFinalizer() = default;
  virtual ~DriverFinalizer() = default;
  void FinalizeDriver();

 public:
  static inline void Finalize() {
    DriverFinalizer finalizer{};
    return finalizer.FinalizeDriver();
  }
};
}  // namespace prt

#endif  // PRT_GFX_DRIVER_H
