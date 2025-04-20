#include "prette/gfx_driver.h"

#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/lua.h"
#include "prette/lua_event.h"
#include "prette/to_string.h"

namespace prt {
static DriverEventSubject events_{};
static Driver* driver_ = nullptr;

auto OnDriverEvent() -> DriverEventObservable {
  return events_.get_observable();
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
  Publish<DestroyingDriverEvent>();
  delete driver_;
  Publish<DriverDestroyedEvent>();
}

void DriverBase::InitDriver() {
  ASSERT(!DriverBase::IsInitialized());
  const auto driver = Driver::New();
  ASSERT(driver);
  SetDriver(driver);
  Publish<DriverInitEvent>();
}

void DriverBase::Init() {
  const auto engine = Engine::Get();
  ASSERT(engine);
  OnPostInitEvent().subscribe([](PostInitEvent* event) {
    ASSERT(event);
    InitDriver();
  });
  OnTerminatedEvent().subscribe([](TerminatedEvent* event) {
    ASSERT(event);
    DestroyDriver();
  });
}
}  // namespace prt