#ifndef PRT_CITIZEN_EVENT_H
#define PRT_CITIZEN_EVENT_H

#include "prette/assertions.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_PERSON_EVENT(V) \
  V(CitizenBornEvent)            \
  V(CitizenDiedEvent)

class CitizenEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_PERSON_EVENT(FORWARD_DECLARE)

class Citizen;
class CitizenEvent : public Event {
 private:
  Citizen* citizen_;

 protected:
  explicit CitizenEvent(Citizen* citizen) :
    Event(),
    citizen_(citizen) {
    ASSERT(citizen_);
  }

 public:
  ~CitizenEvent() override = default;

  auto GetCitizen() const -> Citizen* {
    return citizen_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Citizen, FOR_EACH_PERSON_EVENT);
};

#define DEFINE_PERSON_EVENT(Name)            \
  class Name##Event : public CitizenEvent {  \
   public:                                   \
    explicit Name##Event(Citizen* citizen) : \
      CitizenEvent(citizen) {}               \
    ~Name##Event() override = default;       \
    DECLARE_EVENT_TYPE(CitizenEvent, Name);  \
  };
FOR_EACH_PERSON_EVENT(DEFINE_PERSON_EVENT)
#undef DEFINE_PERSON_EVENT
}  // namespace prt

#endif  // PRT_CITIZEN_EVENT_H
