#ifndef PRT_SETTINGS_H
#define PRT_SETTINGS_H

#include "prette/flags.h"
#include "prette/settings/setting.h"
#include "prette/settings/settings_events.h"

namespace prt::settings {
DECLARE_string(settings_dir);
  class Settings {
  private:
    static void Publish(SettingsEvent* event);

    template<class E, typename... Args>
    static inline void
    Publish(Args... args) {
      E event(args...);
      return Publish((SettingsEvent*) &event);
    }
  public:
    static void Initialize();
    static auto VisitSettings(SettingVisitor* vis) -> bool;

    static inline auto OnEvent() -> rx::observable<SettingsEvent*>;

#define DEFINE_ON_SETTINGS_EVENT(Name)                                \
    static inline auto                                                \
    On##Name##Event() -> rx::observable<Name##Event*> {               \
      return OnEvent()                                                \
        .filter(Name##Event::Filter)                                  \
        .map(Name##Event::Cast);                                      \
    }
    FOR_EACH_SETTINGS_EVENT(DEFINE_ON_SETTINGS_EVENT)
#undef DEFINE_ON_SETTINGS_EVENT
};
}

#endif //PRT_SETTINGS_H