#ifndef PRT_CITY_MANAGER_H
#define PRT_CITY_MANAGER_H

#include <cstdint>
#include <filesystem>
#include <marl/waitgroup.h>
#include <string>
#include <vector>

#include "prette/city.h"
#include "prette/city_storage.h"
#include "prette/common.h"
#include "prette/event.h"
#include "prette/gender.h"
#include "prette/person.h"
#include "prette/rx.h"
#include "prette/tick.h"

namespace prt {
#define FOR_EACH_CITY_MANAGER_EVENT(V) \
  V(CityManagerInit)                   \
  V(CitiesLoaded)                      \
  V(CitiesSaved)                       \
  V(CitiesUpdated)                     \
  V(CityManagerDestroyed)

class CityManagerEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_CITY_MANAGER_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class CityManagerEvent : public Event {
 protected:
  CityManagerEvent() = default;

 public:
  ~CityManagerEvent() override = default;
  DEFINE_EVENT_PROTOTYPE_TYPE(CityManager, FOR_EACH_CITY_MANAGER_EVENT);
};

#define DECLARE_CITY_MANAGER_EVENT(Name)        \
  class Name##Event : public CityManagerEvent { \
   public:                                      \
    Name##Event() = default;                    \
    ~Name##Event() override = default;          \
    DECLARE_EVENT_TYPE(CityManagerEvent, Name); \
  };
FOR_EACH_CITY_MANAGER_EVENT(DECLARE_CITY_MANAGER_EVENT)
#undef DECLARE_CITY_MANAGER_EVENT

DEFINE_EVENT_OBSERVABLE(CityManager);
DEFINE_EVENT_SUBJECT(CityManager);
FOR_EACH_CITY_MANAGER_EVENT(DEFINE_EVENT_OBSERVABLE);

auto GetCityManagerEventObservable() -> CityManagerEventObservable;

template <typename... ArgN>
static inline auto OnCityManagerEvent(ArgN... args) -> rx::composite_subscription {
  return GetCityManagerEventObservable().subscribe(args...);
}

#define DEFINE_ON_EVENT(Name)                                                    \
  auto Get##Name##EventObservable()->Name##EventObservable;                      \
  template <typename... ArgN>                                                    \
  static inline auto On##Name##Event(ArgN... args)->rx::composite_subscription { \
    return Get##Name##EventObservable().subscribe(args...);                      \
  }
FOR_EACH_CITY_MANAGER_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

class CityManager {
  friend class City;
  DEFINE_NON_COPYABLE_TYPE(CityManager);

 private:
  static void Publish(CityManagerEvent* event);

  template <class E, typename... Args>
  static inline void PublishCityManagerEvent(Args... args) {
    E event(args...);
    return Publish(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return PublishCityManagerEvent<Name##Event>(args...); \
  }
  FOR_EACH_CITY_MANAGER_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  CityStorage storage_;
  rx::subscription on_tick_{};
  std::vector<City> cities_{};
  std::vector<std::vector<Person>> citizens_{};

  CityManager(const fs::path root);

  void LoadAllCities();
  void SaveAllCities();
  void UpdateCities(const TickDelta delta);

  inline auto citizens(const CityId city) -> std::vector<Person>& {
    return citizens_[city];
  }

  inline auto citizens(const CityId city) const -> const std::vector<Person>& {
    return citizens_[city];
  }

 public:
  ~CityManager();

  auto GetStorage() const -> const CityStorage& {
    return storage_;
  }

  auto NewCity(const std::string name) -> City*;
  auto NewCitizen(const CityId id, const Gender gender, const std::string forename, const std::string surname)
      -> Person*;
  auto NewCitizen(const CityId id, const Gender gender) -> Person*;

  inline auto NewMaleCitizen(const CityId id, const std::string forename, const std::string surname) -> Person* {
    return NewCitizen(id, Gender::kMale, forename, surname);
  }

  inline auto NewMaleCitizen(const CityId id) -> Person* {
    return NewCitizen(id, Gender::kMale);
  }

  inline auto NewFemaleCitizen(const CityId id, const std::string forename, const std::string surname) -> Person* {
    return NewCitizen(id, Gender::kFemale, forename, surname);
  }

  inline auto NewFemaleCitizen(const CityId id) -> Person* {
    return NewCitizen(id, Gender::kFemale);
  }

  inline auto GetNumberOfCities() const -> uint64_t {
    return cities_.size();
  }

  inline auto GetCityAt(const uint64_t idx) -> City* {
    return &cities_.at(idx);
  }

  inline auto GetAllCities() -> CityObservable {
    return rx::observable<>::create<City*>([this](rx::subscriber<City*> s) {
      for (auto& city : cities_)
        s.on_next(&city);
      s.on_completed();
    });
  }

  template <CityPredicateLike Predicate>
  inline auto VisitAllCities(const Predicate& vis) -> bool {
    for (auto& city : cities_) {
      if (!vis(&city))
        return false;
    }
    return true;
  }

  inline auto GetNumberOfCitizens(const CityId city) const -> uint64_t {
    return citizens(city).size();
  }

  inline auto GetCitizenAt(const CityId city, const uint64_t idx) -> Person* {
    return &citizens(city)[idx];
  }

  inline auto GetAllCitizens(const CityId id) -> PersonObservable {
    return rx::observable<>::create<Person*>([this, id](rx::subscriber<Person*> s) {
      for (auto& citizen : citizens(id))
        s.on_next(&citizen);
      s.on_completed();
    });
  }

  template <PersonPredicateLike Predicate>
  inline auto VisitAllCitizensForCity(const CityId city, const Predicate& vis) -> bool {
    for (auto& city : citizens(city)) {
      if (!vis(&city))
        return false;
    }
    return true;
  }

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> CityManager*;
};
}  // namespace prt

#endif  // PRT_CITY_MANAGER_H
