#ifndef PRT_SETTINGS_EVENT_H
#define PRT_SETTINGS_EVENT_H

#include "prette/event.h"
#include "prette/settings/settings_state.h"

namespace prt {
#define FOR_EACH_SETTINGS_EVENT(V) \
  FOR_EACH_SETTINGS_STATE(V)       \
  V(GenerateSettings)              \
  V(SavingSettings)                \
  V(SettingsSaved)                 \
  V(LoadingSettings)               \
  V(SettingsLoaded)

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
FOR_EACH_SETTINGS_EVENT(DEFINE_SETTINGS_EVENT);
#undef DEFINE_SETTINGS_EVENT

DEFINE_EVENT_SUBJECT(Settings);
DEFINE_EVENT_OBSERVABLE(Settings);
FOR_EACH_SETTINGS_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_SETTINGS_EVENT_H
