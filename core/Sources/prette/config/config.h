#ifndef PRT_CONFIG_H
#define PRT_CONFIG_H

#include <absl/status/status.h>
#include <cstdint>
#include <libconfig.h>  // IWYU pragma: export
#include <optional>
#include <string_view>

#include "prette/config/config_state.h"
#include "prette/config/config_topic.h"
#include "prette/relaxed_atomic.h"

namespace prt {
class Config {
  friend class ConfigFile;
  friend class ConfigSaver;
  friend class ConfigLoader;

 public:
  using Key = std::string_view;

 private:
  RelaxedAtomic<ConfigState> state_ = ConfigState::kNoConfig;
  config_t config_{};
  ConfigTopic topic_{};
  rx::subscription on_pre_init_{};

  auto ReadConfig() -> absl::Status;
  auto WriteConfig() -> absl::Status;

 public:
  Config();
  ~Config();

  auto GetTopic() -> ConfigTopic& {
    return topic_;
  }

  auto GetTopic() const -> const ConfigTopic& {
    return topic_;
  }

  void Save();
  void Load();

  auto GetBool(const Key k) const -> std::optional<bool>;
  auto GetInt(const Key k) const -> std::optional<int32_t>;
  auto GetInt64(const Key k) const -> std::optional<int64_t>;
  auto GetDouble(const Key k) const -> std::optional<double>;
  auto GetString(const Key k) const -> std::optional<const char*>;

  void PutBool(const Key k, const bool value);
  void PutInt(const Key k, const int32_t value);
  void PutInt64(const Key k, const int64_t value);
  void PutDouble(const Key k, const double value);
  void PutString(const Key k, const std::string_view value);
};

void InitConfig();
auto GetConfig() -> Config*;
auto IsConfigInitialized() -> bool;
}  // namespace prt

#endif  // PRT_CONFIG_H
