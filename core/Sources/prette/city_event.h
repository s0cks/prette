#ifndef PRT_CITY_EVENT_H
#define PRT_CITY_EVENT_H

#include "prette/assertions.h"
#include "prette/event.h"
namespace prt {
#define FOR_EACH_CITY_EVENT(V) \
  V(CityCreated)               \
  V(CityLoaded)                \
  V(CitySaved)                 \
  V(CityDestroyed)

class CityEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_CITY_EVENT(FORWARD_DECLARE)

class City;
class CityEvent : public Event {
 private:
  City* city_;

 protected:
  explicit CityEvent(City* city) :
    Event(),
    city_(city) {
    ASSERT(city_);
  }

 public:
  ~CityEvent() override = default;

  auto GetCity() const -> City* {
    return city_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(City, FOR_EACH_CITY_EVENT);
};

#define DECLARE_CITY_EVENT(Name)         \
  class Name##Event : public CityEvent { \
   public:                               \
    explicit Name##Event(City* city) :   \
      CityEvent(city) {}                 \
    ~Name##Event() override = default;   \
    DECLARE_EVENT_TYPE(CityEvent, Name); \
  };
FOR_EACH_CITY_EVENT(DECLARE_CITY_EVENT)
#undef DECLARE_CITY_EVENT

DEFINE_EVENT_OBSERVABLE(City);
DEFINE_EVENT_SUBJECT(City);
FOR_EACH_CITY_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_CITY_EVENT_H
