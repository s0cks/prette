#include "prette/gfx_driver.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/gfx.h"
#include "prette/gfx_driver_event.h"
#include "prette/pipeline/pipeline_layout.h"
#include "prette/thread_local.h"

namespace prt {
static DriverEventSubject events_{};
static ThreadLocal<Driver> driver_{};

static inline auto SetDriver(Driver* driver) -> Driver* {
  ASSERT(driver);
  ASSERT(!Driver::IsInitialized());
  return driver_ = driver;
}

auto OnDriverEvent() -> DriverEventObservable {
  return events_.get_observable();
}

void DriverBase::PublishEvent(DriverEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

void DriverBase::Init() {
  vk::InitPipelineLayoutManager();
  OnPostInitEvent().subscribe([](PostInitEvent* event) {
    DriverInitializer::Init();
  });
}

auto DriverBase::IsInitialized() -> bool {
  return driver_.Get() != nullptr;
}

auto DriverBase::Get() -> Driver* {
  return (Driver*)driver_;
}

void DriverInitializer::InitDriver() {
  ASSERT(!Driver::IsInitialized());
  const auto driver = (Driver*)(malloc(sizeof(Driver)));  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  ASSERT(driver);
  SetDriver(driver);
  new (driver) Driver();
  DriverBase::Publish<DriverInitEvent>();
}

void DriverFinalizer::FinalizeDriver() {
  ASSERT(Driver::IsInitialized());
  driver_->WaitDeviceIdle();
  DriverBase::Publish<DestroyingDriverEvent>();
  delete driver_;
  DriverBase::Publish<DriverDestroyedEvent>();
}
}  // namespace prt