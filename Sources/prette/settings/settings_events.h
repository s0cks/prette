#ifndef PRT_SETTINGS_EVENTS_H
#define PRT_SETTINGS_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::settings {
#define FOR_EACH_SETTINGS_EVENT(V) \
  V(SettingsLoading)               \
  V(SettingsLoaded)                \
  V(SettingsSaving)                \
  V(SettingsSaved)

  class SettingsEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_SETTINGS_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class SettingsEvent : public Event {
  protected:
    SettingsEvent() = default;
  public:
    ~SettingsEvent() override = default;

#define DEFINE_TYPE_CHECK(Name)                                               \
    virtual auto As##Name##Event() -> Name##Event* { return nullptr; }        \
    auto Is##Name##Event() -> bool { return As##Name##Event() != nullptr; }
    FOR_EACH_SETTINGS_EVENT(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  };

#define DECLARE_SETTINGS_EVENT(Name)                                              \
  public:                                                                         \
    auto ToString() const -> std::string override;                                \
    auto GetName() const -> const char* override { return #Name; }                \
    auto As##Name##Event() -> Name##Event* override { return this; }              \
    static inline auto                                                            \
    Filter(SettingsEvent* event) -> bool {                                        \
      return event                                                                \
          && event->Is##Name##Event();                                            \
    }                                                                             \
    static inline auto                                                            \
    Cast(SettingsEvent* event) -> Name##Event* {                                  \
      PRT_ASSERT(event);                                                          \
      PRT_ASSERT(event->Is##Name##Event());                                       \
      return event->As##Name##Event();                                            \
    }

  class SettingsLoadingEvent : public SettingsEvent {
  public:
    SettingsLoadingEvent() = default;
    ~SettingsLoadingEvent() override = default;
    DECLARE_SETTINGS_EVENT(SettingsLoading);
  };

  class SettingsLoadedEvent : public SettingsEvent {
  public:
    SettingsLoadedEvent() = default;
    ~SettingsLoadedEvent() override = default;
    DECLARE_SETTINGS_EVENT(SettingsLoaded);
  };

  class SettingsSavingEvent : public SettingsEvent {
  public:
    SettingsSavingEvent() = default;
    ~SettingsSavingEvent() override = default;
    DECLARE_SETTINGS_EVENT(SettingsSaving);
  };

  class SettingsSavedEvent : public SettingsEvent {
  public:
    SettingsSavedEvent() = default;
    ~SettingsSavedEvent() override = default;
    DECLARE_SETTINGS_EVENT(SettingsSaved);
  };
}

#endif //PRT_SETTINGS_EVENTS_H