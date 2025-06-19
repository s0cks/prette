#ifndef PRT_SETTINGS_TOPIC_H
#define PRT_SETTINGS_TOPIC_H

#include <type_traits>

#include "prette/assertions.h"
#include "prette/settings/settings_event.h"

namespace prt {
template <class E>
struct is_settings_topic_event_t : std::false_type {};

#define DECLARE_SETTINGS_TOPIC_EVENT(Name) \
  template <>                              \
  struct is_settings_topic_event_t<Name##Event> : std::true_type {};
FOR_EACH_SETTINGS_EVENT(DECLARE_SETTINGS_TOPIC_EVENT);
#undef DECLARE_SETTINGS_TOPIC_EVENT

template <class E>
concept SettingsTopicEvent = is_settings_topic_event_t<E>::value;

class SettingsTopic {
 private:
  SettingsInitEventSubject init_{};
  SettingsDeInitEventSubject deinit_{};
  GenerateSettingsEventSubject generate_{};
  SavingSettingsEventSubject saving_{};
  SettingsSavedEventSubject saved_{};
  LoadingSettingsEventSubject loading_{};
  SettingsLoadedEventSubject loaded_{};

 public:
  SettingsTopic() = default;
  ~SettingsTopic() = default;

#define DEFINE_EVENT_FUNCTIONS(Name)                                      \
  inline auto Get##Name##EventObservable() const->Name##EventObservable;  \
  inline operator Name##EventObservable() const {                         \
    return Get##Name##EventObservable();                                  \
  }                                                                       \
  template <typename... ArgN>                                             \
  inline auto On##Name##Event(ArgN... args)->rx::composite_subscription { \
    return Get##Name##EventObservable().subscribe(args...);               \
  }                                                                       \
  template <typename... Args>                                             \
  inline void Publish##Name##Event(Args... args);
  FOR_EACH_SETTINGS_EVENT(DEFINE_EVENT_FUNCTIONS)
#undef DEFINE_EVENT_FUNCTIONS

  template <SettingsTopicEvent E>
  inline void Publish(E* event);
};

#define DEFINE_GET_EVENT_OBSERVABLE(Name, Subject)                                       \
  inline auto SettingsTopic::Get##Name##EventObservable() const->Name##EventObservable { \
    return (Subject).get_observable();                                                   \
  }
DEFINE_GET_EVENT_OBSERVABLE(SettingsInit, init_);
DEFINE_GET_EVENT_OBSERVABLE(SettingsDeInit, deinit_);
DEFINE_GET_EVENT_OBSERVABLE(GenerateSettings, generate_);
DEFINE_GET_EVENT_OBSERVABLE(SavingSettings, saving_);
DEFINE_GET_EVENT_OBSERVABLE(SettingsSaved, saved_);
DEFINE_GET_EVENT_OBSERVABLE(LoadingSettings, loading_);
DEFINE_GET_EVENT_OBSERVABLE(SettingsLoaded, loaded_);
#undef DEFINE_GET_EVENT_OBSERVABLE

#define DEFINE_PUBLISH(Name, Subject)                                    \
  template <>                                                            \
  inline void SettingsTopic::Publish<Name##Event>(Name##Event * event) { \
    ASSERT(event);                                                       \
    const auto& subscriber = (Subject).get_subscriber();                 \
    return subscriber.on_next(event);                                    \
  }                                                                      \
  template <typename... Args>                                            \
  inline void SettingsTopic::Publish##Name##Event(Args... args) {        \
    Name##Event event(args...);                                          \
    return Publish(&event);                                              \
  }
DEFINE_PUBLISH(SettingsInit, init_);
DEFINE_PUBLISH(SettingsDeInit, deinit_);
DEFINE_PUBLISH(GenerateSettings, generate_);
DEFINE_PUBLISH(SavingSettings, saving_);
DEFINE_PUBLISH(SettingsSaved, saved_);
DEFINE_PUBLISH(LoadingSettings, loading_);
DEFINE_PUBLISH(SettingsLoaded, loaded_);
#undef DEFINE_PUBLISH
}  // namespace prt

#endif  // PRT_SETTINGS_TOPIC_H
