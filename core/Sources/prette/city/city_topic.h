#ifndef PRT_CITY_TOPIC_H
#define PRT_CITY_TOPIC_H

#include "prette/assertions.h"
#include "prette/city/city_event.h"
#include "prette/event.h"
#include "prette/rx.h"

namespace prt {
class CityTopic : public Topic {
  friend class City;

 private:
  City* owner_;
  CitySavedEventSubject saved_{};
  CityLoadedEventSubject loaded_{};
  CityUpdatedEventSubject updated_{};

 public:
  explicit CityTopic(City* owner = nullptr) :
    owner_(owner) {}
  ~CityTopic() = default;

  auto GetOwner() const -> City* {
    return owner_;
  }

  inline auto HasOwner() const -> bool {
    return GetOwner() != nullptr;
  }

#define DEFINE_EVENT_FUNCTIONS(Name)                                     \
  inline void Publish##Name##Event();                                    \
  inline auto Get##Name##EventObservable() const->Name##EventObservable; \
  inline operator Name##EventObservable() const {                        \
    return Get##Name##EventObservable();                                 \
  }                                                                      \
  template <typename... ArgN>                                            \
  inline auto On##Name(ArgN... args) const->rx::composite_subscription { \
    return Get##Name##EventObservable().subscribe(args...);              \
  }

  FOR_EACH_CITY_EVENT(DEFINE_EVENT_FUNCTIONS)
#undef DEFINE_EVENT_FUNCTIONS
};

#define DEFINE_GET_TOPIC_OBSERVABLE(Name, Subject)                                   \
  inline auto CityTopic::Get##Name##EventObservable() const->Name##EventObservable { \
    return (Subject).get_observable();                                               \
  }

DEFINE_GET_TOPIC_OBSERVABLE(CitySaved, saved_);
DEFINE_GET_TOPIC_OBSERVABLE(CityLoaded, loaded_);
DEFINE_GET_TOPIC_OBSERVABLE(CityUpdated, updated_);
#undef DEFINE_GET_TOPIC_OBSERVABLE

#define DEFINE_PUBLISH_EVENT(Name, Subject)   \
  void CityTopic::Publish##Name##Event() {    \
    ASSERT(HasOwner());                       \
    Name##Event event(GetOwner());            \
    return PublishEventTo((Subject), &event); \
  }

DEFINE_PUBLISH_EVENT(CitySaved, saved_);
DEFINE_PUBLISH_EVENT(CityLoaded, loaded_);
DEFINE_PUBLISH_EVENT(CityUpdated, updated_);
#undef DEFINE_PUBLISH_EVENT

#define DEFINE_PUBLISH_EVENT(Name, Subject)   \
  void CityTopic::Publish##Name##Event() {    \
    ASSERT(HasOwner());                       \
    Name##Event event{};                      \
    return PublishEventTo((Subject), &event); \
  }
#undef DEFINE_PUBLISH_EVENT
}  // namespace prt

#endif  // PRT_CITY_TOPIC_H
