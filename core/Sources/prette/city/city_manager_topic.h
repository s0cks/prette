#ifndef PRT_CITY_MANAGER_TOPIC_H
#define PRT_CITY_MANAGER_TOPIC_H

#include <string>

#include "prette/assertions.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_CITY_MANAGER_TOPIC_EVENT(V) V(CityChangedEvent)

class City;
class CityChangedEvent : public Event {
 private:
  City* previous_;
  City* current_;

 public:
  CityChangedEvent(City* previous, City* current) :
    Event(),
    previous_(previous),
    current_(current) {
    ASSERT(current_);
  }
  ~CityChangedEvent() override = default;

  auto GetName() const -> const char* override {
    return "CityChanged";
  }

  auto GetPreviousCity() const -> City* {
    return previous_;
  }

  auto GetCurrentCity() const -> City* {
    return current_;
  }

  auto ToString() const -> std::string override;
};

DEFINE_EVENT_SUBJECT(CityChanged);
DEFINE_EVENT_OBSERVABLE(CityChanged);

class CityManagerTopic : public Topic {
 private:
  CityChangedEventSubject changed_{};

 public:
  CityManagerTopic() = default;
  ~CityManagerTopic() = default;

  inline auto GetCityChangedEventObservable() const -> CityChangedEventObservable {
    return changed_.get_observable();
  }

  inline operator CityChangedEventObservable() const {
    return GetCityChangedEventObservable();
  }

  template <typename... ArgN>
  inline auto OnCityChanged(ArgN... args) const -> rx::composite_subscription {
    return GetCityChangedEventObservable().subscribe(args...);
  }

  inline void PublishCityChangedEvent(City* previous, City* current) const {
    ASSERT(current);
    CityChangedEvent event(previous, current);
    return PublishEventTo(changed_, &event);
  }
};
}  // namespace prt

#endif  // PRT_CITY_MANAGER_TOPIC_H
