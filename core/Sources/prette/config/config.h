#ifndef PRT_CONFIG_H
#define PRT_CONFIG_H

#include <libconfig.h>  // IWYU pragma: export
#include <ostream>
#include <string>

#include "prette/config/config_event.h"
#include "prette/config/config_state.h"
#include "prette/relaxed_atomic.h"
#include "prette/rx.h"

namespace prt {
auto GetConfigEventObservable() -> ConfigEventObservable;
template <typename... ArgN>
static inline auto OnConfigEvent(ArgN... args) -> rx::composite_subscription {
  return GetConfigEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                                             \
  template <typename... ArgN>                                                                             \
  static inline auto OnConfig##Name##Event()->Config##Name##EventObservable {                             \
    return GetConfigEventObservable().filter(Config##Name##Event::Filter).map(Config##Name##Event::Cast); \
  }                                                                                                       \
  template <typename... ArgN>                                                                             \
  static inline auto OnConfig##Name(ArgN... args)->rx::composite_subscription {                           \
    return OnConfig##Name##Event().subscribe(args...);                                                    \
  }
FOR_EACH_CONFIG_STATE(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class Config {
  friend class ConfigFile;
  friend class ConfigSaver;
  friend class ConfigLoader;
  friend class ConfigSystem;

  class Status {
   private:
    int value_;
    std::string message_;

   public:
    Status(config_t* config, const int value) :
      value_(value),
      message_() {
      if (value != CONFIG_TRUE)
        message_ = config_error_text(config);
    }
    Status(const int value) :
      value_(value),
      message_() {}
    ~Status() = default;

    auto GetStatus() const -> int {
      return value_;
    }

    inline auto IsOk() const -> bool {
      return GetStatus() == CONFIG_TRUE;
    }

    auto GetMessage() const -> const std::string& {
      return message_;
    }

    operator bool() const {
      return IsOk();
    }

    operator int() const {
      return value_;
    }

    operator std::string() const {
      return message_;
    }

    friend auto operator<<(std::ostream& stream, const Status& rhs) -> std::ostream& {
      if (rhs.IsOk())
        return stream << "ok";
      return stream << "config error: " << rhs.GetMessage();
    }
  };

  static void PublishEvent(ConfigEvent* event);

  template <class E, typename... Args>
  static inline void Publish(Args... args) {
    E event(args...);
    return PublishEvent(&event);
  }

#define DEFINE_PUBLISH(Name)                                    \
  template <typename... Args>                                   \
  static inline auto PublishConfig##Name##Event(Args... args) { \
    return Publish<Config##Name##Event>(args...);               \
  }
  FOR_EACH_CONFIG_STATE(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  RelaxedAtomic<ConfigState> state_ = ConfigState::kNoConfig;
  config_t config_{};

  Config() = default;
  ~Config() = default;

  auto Save() -> Status;
  auto Load() -> Status;

 public:
  auto GetString(std::string key) const -> std::string;
};
}  // namespace prt

#endif  // PRT_CONFIG_H
