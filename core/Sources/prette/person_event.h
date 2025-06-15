#ifndef PRT_PERSON_EVENT_H
#define PRT_PERSON_EVENT_H

#include "prette/assertions.h"
#include "prette/event.h"

namespace prt {
#define FOR_EACH_PERSON_EVENT(V) \
  V(PersonBornEvent)             \
  V(PersonDiedEvent)

class PersonEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_PERSON_EVENT(FORWARD_DECLARE)

class Person;
class PersonEvent : public Event {
 private:
  Person* person_;

 protected:
  explicit PersonEvent(Person* person) :
    Event(),
    person_(person) {
    ASSERT(person_);
  }

 public:
  ~PersonEvent() override = default;

  auto GetPerson() const -> Person* {
    return person_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Person, FOR_EACH_PERSON_EVENT);
};

#define DEFINE_PERSON_EVENT(Name)          \
  class Name##Event : public PersonEvent { \
   public:                                 \
    explicit Name##Event(Person* person) : \
      PersonEvent(person) {}               \
    ~Name##Event() override = default;     \
    DECLARE_EVENT_TYPE(PersonEvent, Name); \
  };
FOR_EACH_PERSON_EVENT(DEFINE_PERSON_EVENT)
#undef DEFINE_PERSON_EVENT
}  // namespace prt

#endif  // PRT_PERSON_EVENT_H
