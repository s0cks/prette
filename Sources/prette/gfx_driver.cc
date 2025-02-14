#include "prette/gfx_driver.h"

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/to_string.h"

namespace prt {
static DriverEventSubject events_{};
static Driver* driver_ = nullptr;

auto OnDriverEvent() -> DriverEventObservable {
  return events_.get_observable();
}

auto DriverCreatedEvent::ToString() const -> std::string {
  ToStringHelper<DriverCreatedEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto InstanceInitEvent::ToString() const -> std::string {
  ToStringHelper<InstanceInitEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto PhysicalDeviceInitEvent::ToString() const -> std::string {
  ToStringHelper<PhysicalDeviceInitEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto SurfaceInitEvent::ToString() const -> std::string {
  ToStringHelper<SurfaceInitEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto DeviceInitEvent::ToString() const -> std::string {
  ToStringHelper<DeviceInitEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto DriverInitEvent::ToString() const -> std::string {
  ToStringHelper<DriverInitEvent> helper;
  helper.AddFieldPtr("driver", GetDriver());
  return helper;
}

auto DestroyingDriverEvent::ToString() const -> std::string {
  ToStringHelper<DestroyingDriverEvent> helper{};
  helper.AddFieldRef("driver", GetDriver());
  return helper;
}

auto DriverDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<DriverDestroyedEvent>{};
}

void DriverBase::PublishEvent(DriverEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto DriverBase::IsInitialized() -> bool {
  return driver_ != nullptr;
}

auto DriverBase::Get() -> Driver* {
  ASSERT(IsInitialized());
  return driver_;
}

static inline auto SetDriver(Driver* driver) -> Driver* {
  ASSERT(driver);
  ASSERT(!DriverBase::IsInitialized());
  return driver_ = driver;
}

void DriverBase::DestroyDriver() {
  ASSERT(driver_);
  driver_->WaitDeviceIdle();
  Publish<DestroyingDriverEvent>(driver_);
  delete driver_;
  Publish<DriverDestroyedEvent>();
}

void DriverBase::InitDriver() {
  ASSERT(!DriverBase::IsInitialized());
  const auto driver = Driver::New();
  ASSERT(driver);
  SetDriver(driver);
  Publish<DriverInitEvent>(driver);
}

void DriverBase::Init() {
  const auto engine = Engine::Get();
  ASSERT(engine);
  engine::OnPostInitEvent().subscribe([](engine::PostInitEvent* event) {
    ASSERT(event);
    InitDriver();
  });
  engine::OnTerminatedEvent().subscribe([](engine::TerminatedEvent* event) {
    ASSERT(event);
    DestroyDriver();
  });
}

#define LUA_DRIVER_F(Name) LUA_F(driver_##Name)

LUA_DRIVER_F(onEvent) {
  OnDriverEvent().subscribe(CreateSubscriber<DriverEvent>(L));
  return 0;
}

#define DEFINE_ON_EVENT_FUNC(Name)                                 \
  LUA_DRIVER_F(on##Name##Event) {                                  \
    On##Name##Event().subscribe(CreateSubscriber<Name##Event>(L)); \
    return 0;                                                      \
  }
FOR_EACH_DRIVER_EVENT(DEFINE_ON_EVENT_FUNC);
#undef DEFINE_ON_EVENT_FUNC

#undef LUA_DRIVER_F

// clang-format off
// NOLINTNEXTLINE
static const struct luaL_Reg kDriverLib[] = {
#define LUA_DRIVER_F(Name) \
  { .name = #Name, .func = &lua_driver_##Name }

  LUA_DRIVER_F(onEvent),
#define DEFINE_ON_EVENT(Name) \
  LUA_DRIVER_F(on##Name##Event),
FOR_EACH_DRIVER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
#undef LUA_DRIVER_F
};
// clang-format on

void DriverBase::InitLua(lua_State* L) {
  ASSERT(L);
  DLOG(INFO) << "initializing lua bindings....";
  lua_newtable(L);
  luaL_setfuncs(L, kDriverLib, 0);
  lua_setglobal(L, "Driver");
}
}  // namespace prt