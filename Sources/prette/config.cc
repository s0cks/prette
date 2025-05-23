#include "prette/config.h"

#include <filesystem>
#include <gflags/gflags.h>
#include <libconfig.h>
#include <string>
#include <utility>

#include "prette/common.h"
#include "prette/config_event.h"

namespace prt {
DEFINE_string(config, kDefaultConfigFilename, "The name of the config file to load.");

static ConfigEventSubject events_{};

auto OnConfigEvent() -> ConfigEventObservable {
  return events_.get_observable();
}

void Config::PublishEvent(ConfigEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

Config::Config(fs::path path) :
  path_(std::move(path)) {
  LoadIfExists();
}

Config::~Config() {
  Save();
}

void Config::Load() {
  if (config_read_file(&config_, GetPath().c_str()) != CONFIG_TRUE)
    LOG(FATAL) << "failed to read config from: " << GetPath();
  Config::Publish<ConfigLoadEvent>();
}

void Config::Save() {
  if (config_write_file(&config_, GetPath().c_str()) != CONFIG_TRUE)
    LOG(FATAL) << "failed to parse config " << GetPath() << ": " << config_error_text(&config_);
  Config::Publish<ConfigSaveEvent>();
}

auto Config::GetString(std::string key) const -> std::string {
  const char* value = nullptr;
  config_lookup_string(&config_, key.c_str(), &value);
  return {value};
}
}  // namespace prt