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
  protected:
    const Setting* setting_;

    explicit SettingEvent(const Setting* setting):
      Event(),
      setting_(setting) {
    }
  public:
    ~SettingEvent() override = default;

    const Setting* GetSetting() const {
      return setting_;
    }
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_SETTING_EVENT);
  };

  typedef rx::subject<SettingEvent*> SettingEventSubject;

#define DECLARE_SETTING_EVENT(Name)                                             \
  DECLARE_EVENT_TYPE(SettingEvent, Name)

  class SettingChangedEvent : public SettingEvent {
  protected:
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

    uword GetOldValueAddress() const {
      return old_;
    }

    template<typename T>
    T* GetOld() const {
      return (T*) GetOldValueAddress();
    }

    uword GetNewValueAddress() const {
      return new_;
    }

    template<typename T>
    T* GetNew() const {
      return (T*) GetNewValueAddress();
    }
    DECLARE_SETTING_EVENT(SettingChanged);
  };

  class SettingEventPublisher : public EventPublisher<SettingEvent> {
  protected:
    SettingEventPublisher() = default;
  public:
    ~SettingEventPublisher() override = default;

#define DEFINE_ON_SETTING_EVENT(Name)             \
    inline rx::observable<Name##Event*>           \
    On##Name##Event() const {                     \
      return OnEvent()                            \
        .filter(Name##Event::Filter)              \
        .map(Name##Event::Cast);                  \
    }
    FOR_EACH_SETTING_EVENT(DEFINE_ON_SETTING_EVENT)
#undef DEFINE_ON_SETTING_EVENT
  };
}

#endif //PRT_SETTING_EVENT_H