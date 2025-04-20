#include "prette/gfx_driver_event.h"

#include "prette/to_string.h"

namespace prt {
auto DriverCreatedEvent::ToString() const -> std::string {
  return ToStringHelper<DriverCreatedEvent>{};
}

auto InstanceInitEvent::ToString() const -> std::string {
  return ToStringHelper<InstanceInitEvent>{};
}

auto PhysicalDeviceInitEvent::ToString() const -> std::string {
  return ToStringHelper<PhysicalDeviceInitEvent>{};
}

auto SurfaceInitEvent::ToString() const -> std::string {
  return ToStringHelper<SurfaceInitEvent>{};
}

auto DeviceInitEvent::ToString() const -> std::string {
  return ToStringHelper<DeviceInitEvent>{};
}

auto DriverInitEvent::ToString() const -> std::string {
  return ToStringHelper<DriverInitEvent>{};
}

auto DestroyingDriverEvent::ToString() const -> std::string {
  return ToStringHelper<DestroyingDriverEvent>{};
}

auto DriverDestroyedEvent::ToString() const -> std::string {
  return ToStringHelper<DriverDestroyedEvent>{};
}
}  // namespace prt