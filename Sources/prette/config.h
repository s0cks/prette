#ifndef PRT_CONFIG_H
#define PRT_CONFIG_H

#include <filesystem>
#include <libconfig.h>  // IWYU pragma: export
#include <string>

#include "prette/common.h"
#include "prette/config_event.h"
#include "prette/flags.h"

namespace prt {
DECLARE_string(config);
static constexpr const auto kDefaultConfigFilename = "prette.cfg";

static inline auto GetConfigPath() -> fs::path {
  ASSERT(!FLAGS_config.empty());
  return FLAGS_config;
}

auto OnConfigEvent() -> ConfigEventObservable;
#define DEFINE_ON_EVENT(Name)                                                  \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnConfigEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_CONFIG_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class Config {
  friend class ConfigFile;
  static void PublishEvent(ConfigEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

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
  explicit Config(fs::path path);
  ~Config();

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  auto Exists() const -> bool {
    return fs::exists(GetPath()) && fs::is_regular_file(GetPath());
  }

  auto GetString(std::string key) const -> std::string;
};
}  // namespace prt

#endif  // PRT_CONFIG_H
