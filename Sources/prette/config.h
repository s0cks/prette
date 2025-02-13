#ifndef PRT_CONFIG_H
#define PRT_CONFIG_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_CONFIG_EVENT(V) \
  V(ConfigLoaded)                \
  V(ConfigSaved)

class ConfigEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_CONFIG_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class ConfigEvent : public Event {
 protected:
  ConfigEvent() = default;

 public:
  ~ConfigEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(Config, FOR_EACH_CONFIG_EVENT);
};

class ConfigLoadedEvent : public ConfigEvent {
 public:
  ConfigLoadedEvent() = default;
  ~ConfigLoadedEvent() override = default;
  DECLARE_EVENT_TYPE(ConfigEvent, ConfigLoaded);
};

class ConfigSavedEvent : public ConfigEvent {
 public:
  ConfigSavedEvent() = default;
  ~ConfigSavedEvent() override = default;
  DECLARE_EVENT_TYPE(ConfigEvent, ConfigSaved);
};

DEFINE_EVENT_SUBJECT(Config);
DEFINE_EVENT_OBSERVABLE(Config);
FOR_EACH_CONFIG_EVENT(DEFINE_EVENT_OBSERVABLE);

auto OnConfigEvent() -> ConfigEventObservable;

#define DEFINE_ON_EVENT(Name)                                                  \
  static inline auto On##Name##Event()->Name##EventObservable {                \
    return OnConfigEvent().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }
FOR_EACH_CONFIG_EVENT(DEFINE_ON_EVENT);
#undef DEFINE_ON_EVENT

class Config {
 public:
  static void Load();
  static void Save();
};
}  // namespace prt

#endif  // PRT_CONFIG_H
