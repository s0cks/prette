#include "prette/config.h"

#include <glog/logging.h>
#include <libconfig.h>

#include "prette/to_string.h"

namespace prt {
static constexpr const auto kConfigFilename = "prette.cfg";

static config_t config_{};
static ConfigEventSubject events_{};

static inline void PublishEvent(ConfigEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

auto OnConfigEvent() -> ConfigEventObservable {
  return events_.get_observable();
}

auto ConfigLoadedEvent::ToString() const -> std::string {
  ToStringHelper<ConfigLoadedEvent> helper{};
  return helper;
}

auto ConfigSavedEvent::ToString() const -> std::string {
  ToStringHelper<ConfigSavedEvent> helper{};
  return helper;
}

void Config::Save() {
  if (config_write_file(&config_, kConfigFilename) != CONFIG_TRUE)
    LOG(FATAL) << "failed to parse config " << kConfigFilename << ": " << config_error_text(&config_);
  Publish<ConfigSavedEvent>();
}

void Config::Load() {
  if (config_read_file(&config_, kConfigFilename) != CONFIG_TRUE)
    LOG(FATAL) << "failed to read config from: " << kConfigFilename;

  // MOTD
  const char* motd = nullptr;
  config_lookup_string(&config_, "motd", &motd);
  DLOG(INFO) << "MOTD: " << motd;

  Publish<ConfigLoadedEvent>();
}
}  // namespace prt