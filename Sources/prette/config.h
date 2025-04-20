#ifndef PRT_CONFIG_H
#define PRT_CONFIG_H

#include <libconfig.h>

#include "prette/common.h"
#include "prette/flags.h"

namespace prt {
DECLARE_string(config);
static constexpr const auto kDefaultConfigFilename = "prette.cfg";

class ConfigFile {
 private:
  fs::path path_;
  config_t config_{};

  void Save();
  void Load();

  inline void LoadIfExists() {
    if (Exists())
      Load();
  }

 public:
  explicit ConfigFile(fs::path path);
  ~ConfigFile();

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  auto Exists() const -> bool {
    return fs::exists(GetPath()) && fs::is_regular_file(GetPath());
  }

  auto GetString(std::string key) const -> std::string;
};

class Config {
  DEFINE_NON_INSTANTIABLE_TYPE(Config);

 public:
  static auto IsInitialized() -> bool;
  static void Init();
  static void DeInit();
  static auto Get() -> ConfigFile*;
};
}  // namespace prt

#endif  // PRT_CONFIG_H
