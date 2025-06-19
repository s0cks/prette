#ifndef PRT_CONFIG_TOPIC_H
#define PRT_CONFIG_TOPIC_H

#include <type_traits>

#include "prette/assertions.h"
#include "prette/config/config_event.h"

namespace prt {
#define FOR_EACH_CONFIG_TOPIC_EVENT(V) FOR_EACH_CONFIG_EVENT(V)

template <class E>
struct is_config_topic_event_t : std::false_type {};

template <class E>
concept ConfigTopicEvent = is_config_topic_event_t<E>::value;

#define DECLARE_CONFIG_TOPIC_EVENT(Name) \
  template <>                            \
  struct is_config_topic_event_t<Name##Event> : std::true_type {};
FOR_EACH_CONFIG_TOPIC_EVENT(DECLARE_CONFIG_TOPIC_EVENT)
#undef DECLARE_CONFIG_TOPIC_EVENT

class ConfigTopic {
 private:
  ConfigLoadingEventSubject loading_{};
  ConfigLoadedEventSubject loaded_{};
  ConfigGeneratedEventSubject generated_{};
  ConfigSavedEventSubject saved_{};
  ConfigSavingEventSubject saving_{};

 public:
  ConfigTopic() = default;
  ~ConfigTopic() = default;

#define DECLARE_EVENT_FUNCTIONS(Name)                                     \
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
  FOR_EACH_CONFIG_TOPIC_EVENT(DECLARE_EVENT_FUNCTIONS)
#undef DECLARE_EVENT_FUNCTIONS

  template <ConfigTopicEvent E>
  inline void Publish(E* event);
};

#define DEFINE_GET_EVENT_OBSERVABLE(Name, Subject)                                     \
  inline auto ConfigTopic::Get##Name##EventObservable() const->Name##EventObservable { \
    return (Subject).get_observable();                                                 \
  }
DEFINE_GET_EVENT_OBSERVABLE(ConfigLoading, loading_);
DEFINE_GET_EVENT_OBSERVABLE(ConfigLoaded, loaded_);
DEFINE_GET_EVENT_OBSERVABLE(ConfigGenerated, generated_);
DEFINE_GET_EVENT_OBSERVABLE(ConfigSaving, saving_);
DEFINE_GET_EVENT_OBSERVABLE(ConfigSaved, saved_);
#undef DEFINE_GET_EVENT_OBSERVABLE

#define DEFINE_PUBLISH(Name, Subject)                                  \
  template <typename... Args>                                          \
  inline void ConfigTopic::Publish##Name##Event(Args... args) {        \
    Name##Event event(args...);                                        \
    return Publish<Name##Event>(&event);                               \
  }                                                                    \
  template <>                                                          \
  inline void ConfigTopic::Publish<Name##Event>(Name##Event * event) { \
    ASSERT(event);                                                     \
    const auto& subscriber = (Subject).get_subscriber();               \
    return subscriber.on_next(event);                                  \
  }
DEFINE_PUBLISH(ConfigLoading, loading_);
DEFINE_PUBLISH(ConfigLoaded, loaded_);
DEFINE_PUBLISH(ConfigGenerated, generated_);
DEFINE_PUBLISH(ConfigSaving, saving_);
DEFINE_PUBLISH(ConfigSaved, saved_);
#undef DEFINE_PUBLISH
}  // namespace prt

#endif  // PRT_CONFIG_TOPIC_H
