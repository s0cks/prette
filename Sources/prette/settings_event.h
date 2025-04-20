#ifndef PRT_SETTINGS_EVENT_H
#define PRT_SETTINGS_EVENT_H

#include "prette/event.h"

namespace prt {
#define FOR_EACH_SETTINGS_EVENT(V) \
  V(SettingsLoaded)                \
  V(SettingsSaved)

class SettingsEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_SETTINGS_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

DEFINE_EVENT_PROTOTYPE(Settings, FOR_EACH_SETTINGS_EVENT);

#define DEFINE_SETTINGS_EVENT_TYPE(Name) DECLARE_EVENT_TYPE(SettingsEvent, Name)

#define DEFINE_SETTINGS_EVENT(Name)          \
  class Name##Event : public SettingsEvent { \
   public:                                   \
    Name##Event() = default;                 \
    ~Name##Event() override = default;       \
    DEFINE_SETTINGS_EVENT_TYPE(Name);        \
  };

DEFINE_SETTINGS_EVENT(SettingsLoaded);
DEFINE_SETTINGS_EVENT(SettingsSaved);

DEFINE_EVENT_SUBJECT(Settings);
DEFINE_EVENT_OBSERVABLE(Settings);
FOR_EACH_SETTINGS_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_SETTINGS_EVENT_H
