#ifndef PRT_CONFIG_EVENT_H
#define PRT_CONFIG_EVENT_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_CONFIG_EVENT(V) \
  V(ConfigLoad)                  \
  V(ConfigSave)

#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_CONFIG_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Config, FOR_EACH_CONFIG_EVENT);

class ConfigLoadEvent : public ConfigEvent {
 public:
  ConfigLoadEvent() = default;
  ~ConfigLoadEvent() override = default;
  DECLARE_EVENT_TYPE(ConfigEvent, ConfigLoad);
};

class ConfigSaveEvent : public ConfigEvent {
 public:
  ConfigSaveEvent() = default;
  ~ConfigSaveEvent() override = default;
  DECLARE_EVENT_TYPE(ConfigEvent, ConfigSave);
};

DEFINE_EVENT_SUBJECT(Config);
DEFINE_EVENT_OBSERVABLE(Config);
FOR_EACH_CONFIG_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_CONFIG_EVENT_H
