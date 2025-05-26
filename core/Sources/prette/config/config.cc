#include "prette/config/config.h"

#include <filesystem>
#include <gflags/gflags.h>
#include <libconfig.h>
#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config_event.h"
#include "prette/config/config_flags.h"
#include "prette/event.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(ConfigEvent, events);

void Config::PublishEvent(ConfigEvent* event) {
  ASSERT(event);
  const auto& subscriber = events.get_subscriber();
  return subscriber.on_next(event);
}

auto Config::Save() -> Status {
  return {&config_, config_write_file(&config_, GetConfigPath().c_str())};
}

auto Config::Load() -> Status {
  return {&config_, config_read_file(&config_, GetConfigPath().c_str())};
}

auto Config::GetString(std::string key) const -> std::string {
  ASSERT(!key.empty());
  const char* value = nullptr;
  config_lookup_string(&config_, key.c_str(), &value);
  return {value};
}
}  // namespace prt