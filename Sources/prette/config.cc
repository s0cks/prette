#include "prette/config.h"

#include "prette/common.h"
#include "prette/thread_local.h"

namespace prt {
DEFINE_string(config, kDefaultConfigFilename, "The name of the config file to load.");

static inline auto GetConfigPath() -> fs::path {
  ASSERT(!FLAGS_config.empty());
  return FLAGS_config;
}

ConfigFile::ConfigFile(fs::path path) :
  path_(std::move(path)) {
  LoadIfExists();
}

ConfigFile::~ConfigFile() {
  Save();
}

void ConfigFile::Load() {
  if (config_read_file(&config_, GetPath().c_str()) != CONFIG_TRUE)
    LOG(FATAL) << "failed to read config from: " << GetPath();
}

void ConfigFile::Save() {
  if (config_write_file(&config_, GetPath().c_str()) != CONFIG_TRUE)
    LOG(FATAL) << "failed to parse config " << GetPath() << ": " << config_error_text(&config_);
}

auto ConfigFile::GetString(std::string key) const -> std::string {
  const char* value = nullptr;
  config_lookup_string(&config_, key.c_str(), &value);
  return {value};
}

static ThreadLocal<ConfigFile> config_{};

auto Config::IsInitialized() -> bool {
  return config_.Get() != nullptr;
}

auto Config::Get() -> ConfigFile* {
  ASSERT(IsInitialized());
  return config_;
}

void Config::Init() {
  ASSERT(!IsInitialized());
  config_ = new ConfigFile(GetConfigPath());
  ASSERT(IsInitialized());
}

void Config::DeInit() {
  ASSERT(IsInitialized());
  delete config_.Get();
  config_.Set(nullptr);
}
}  // namespace prt