#ifndef PRT_SETTING_EVENT_H
#define PRT_SETTING_EVENT_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::settings {
#define FOR_EACH_SETTING_EVENT(V)           \
  V(SettingChanged)

  class Setting;
  class SettingEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_SETTING_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class SettingEvent : public Event {
    DEFINE_NON_COPYABLE_TYPE(SettingEvent);
  private:
    const Setting* setting_;
  protected:
    explicit SettingEvent(const Setting* setting):
      Event(),
      setting_(setting) {
    }
  public:
    ~SettingEvent() override = default;

    auto GetSetting() const -> const Setting* {
      return setting_;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_SETTING_EVENT);
  };

#define DECLARE_SETTING_EVENT(Name)         \
  DECLARE_EVENT_TYPE(SettingEvent, Name)

  class SettingChangedEvent : public SettingEvent {
  private:
    uword old_;
    uword new_;
  public:
    explicit SettingChangedEvent(const Setting* setting,
                                 const uword old_value,
                                 const uword new_value):
      SettingEvent(setting),
      old_(old_value),
      new_(new_value) {
    }
    ~SettingChangedEvent() override = default;

    auto GetOldValueAddress() const -> uword {
      return old_;
    }

    template<typename T>
    auto GetOld() const -> T* {
      return (T*) GetOldValueAddress();
    }

    auto GetNewValueAddress() const -> uword {
      return new_;
    }

    template<typename T>
    auto GetNew() const -> T* {
      return (T*) GetNewValueAddress();
    }
    DECLARE_SETTING_EVENT(SettingChanged);
  };

  using SettingEventSubject = rx::subject<SettingEvent*>;
  using SettingEventObservable = rx::observable<SettingEvent*>;
#define DEFINE_EVENT_OBSERVABLE(Name)   \
  using Name##EventObservable = rx::observable<Name##Event*>;
  FOR_EACH_SETTING_EVENT(DEFINE_EVENT_OBSERVABLE)
#undef DEFINE_EVENT_OBSERVABLE

  class SettingEventSource : public EventSource<SettingEvent> {
    DEFINE_NON_COPYABLE_TYPE(SettingEventSource);
  protected:
    SettingEventSource() = default;
  public:
    ~SettingEventSource() override = default;
#define DEFINE_ON_EVENT(Name)                           \
    auto On##Name() const -> Name##EventObservable {    \
      return OnEvent()                                  \
        .filter(Name##Event::Filter)                    \
        .map(Name##Event::Cast);                        \
    }
    FOR_EACH_SETTING_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_SETTING_EVENT_H