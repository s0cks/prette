#include "prette/config/config.h"

#include <absl/status/status.h>
#include <filesystem>
#include <gflags/gflags.h>
#include <libconfig.h>
#include <optional>
#include <sstream>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config_flags.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<Config> config_{};

void InitConfig() {
  ASSERT(!IsConfigInitialized());
  config_ = new Config();
}

auto GetConfig() -> Config* {
  ASSERT(IsConfigInitialized());
  return config_.Get();
}

auto IsConfigInitialized() -> bool {
  return config_.Get() != nullptr;
}

Config::Config() {
  on_pre_init_ = OnPreInit([this](PreInitEvent* event) {
    return Load();
  });
}

Config::~Config() {
  on_pre_init_.unsubscribe();
}

auto Config::WriteConfig() -> absl::Status {
  const auto status = config_write_file(&config_, GetConfigPath().c_str());
  if (status == CONFIG_TRUE)
    return absl::OkStatus();
  ASSERT(status == CONFIG_FALSE);
  std::stringstream ss;
  ss << config_error_type(&config_) << " ";
  ss << config_error_file(&config_) << ":" << config_error_line(&config_) << "] " << config_error_text(&config_);
  return {absl::StatusCode::kInternal, ss.str()};
}

auto Config::ReadConfig() -> absl::Status {
  const auto status = config_read_file(&config_, GetConfigPath().c_str());
  if (status == CONFIG_TRUE)
    return absl::OkStatus();
  ASSERT(status == CONFIG_FALSE);
  std::stringstream ss;
  ss << config_error_type(&config_) << " ";
  ss << config_error_file(&config_) << ":" << config_error_line(&config_) << "] " << config_error_text(&config_);
  return {absl::StatusCode::kInternal, ss.str()};
}

void Config::Save() {
  SCOPE_PROFILER(save);
  topic_.PublishConfigSavingEvent();
  const auto status = WriteConfig();
  LOG_IF(FATAL, !status.ok()) << "failed to write config: " << status.ToString();
  topic_.PublishConfigSavedEvent();
}

void Config::Load() {
  SCOPE_PROFILER(load);
  topic_.PublishConfigLoadingEvent();
  const auto status = ReadConfig();
  LOG_IF(FATAL, !status.ok()) << "failed to read config: " << status.ToString();
  topic_.PublishConfigLoadedEvent();
}

auto Config::GetBool(const Key k) const -> std::optional<bool> {
  ASSERT_NOT_EMPTY(k);
  int value = 0;
  const auto status = config_lookup_bool(&config_, k.data(), &value);
  return status == CONFIG_TRUE ? std::optional<bool>{static_cast<bool>(value)} : std::nullopt;
}

auto Config::GetInt(const Key k) const -> std::optional<int32_t> {
  ASSERT_NOT_EMPTY(k);
  int32_t value = 0;
  const auto status = config_lookup_int(&config_, k.data(), &value);
  return status == CONFIG_TRUE ? std::optional<int32_t>{value} : std::nullopt;
}

auto Config::GetInt64(const Key k) const -> std::optional<int64_t> {
  ASSERT_NOT_EMPTY(k);
  int64_t value = 0;
  const auto status = config_lookup_int64(&config_, k.data(), &value);
  return status == CONFIG_TRUE ? std::optional<int64_t>{value} : std::nullopt;
}

auto Config::GetDouble(const Key k) const -> std::optional<double> {
  ASSERT_NOT_EMPTY(k);
  double value = 0;
  const auto status = config_lookup_float(&config_, k.data(), &value);
  return status == CONFIG_TRUE ? std::optional<double>{value} : std::nullopt;
}

auto Config::GetString(const Key k) const -> std::optional<const char*> {
  ASSERT_NOT_EMPTY(k);
  const char* value = nullptr;
  const auto status = config_lookup_string(&config_, k.data(), &value);
  return status == CONFIG_TRUE ? std::optional<const char*>{value} : std::nullopt;
}
}  // namespace prt