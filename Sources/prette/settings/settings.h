#ifndef PRT_SETTINGS_H
#define PRT_SETTINGS_H

#include "prette/flags.h"
#include "prette/settings/setting.h"
#include "prette/settings/settings_events.h"

namespace prt {
  namespace settings {
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
      static bool VisitSettings(SettingVisitor* vis);

      static inline rx::observable<SettingsEvent*> OnEvent();

#define DEFINE_ON_SETTINGS_EVENT(Name)                                  \
      static inline rx::observable<Name##Event*>                        \
      On##Name##Event() {                                               \
        return OnEvent()                                                \
          .filter(Name##Event::Filter)                                  \
          .map(Name##Event::Cast);                                      \
      }
      FOR_EACH_SETTINGS_EVENT(DEFINE_ON_SETTINGS_EVENT)
#undef DEFINE_ON_SETTINGS_EVENT
    };
  }
}

#endif //PRT_SETTINGS_H