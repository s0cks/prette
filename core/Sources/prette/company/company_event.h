#ifndef PRT_COMPANY_EVENT_H
#define PRT_COMPANY_EVENT_H

#include "prette/event.h"
namespace prt {
#define FOR_EACH_COMPANY_EVENT(V) V(CompanyCreated)

class Company;
class CompanyEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_COMPANY_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class CompanyEvent : public Event {
 private:
  const Company* company_;

 protected:
  CompanyEvent(const Company* company) :
    Event(),
    company_(company) {}

 public:
  ~CompanyEvent() override = default;

  auto GetCompany() const -> const Company* {
    return company_;
  }

  DEFINE_EVENT_PROTOTYPE_TYPE(Company, FOR_EACH_COMPANY_EVENT);
};

#define DECLARE_COMPANY_EVENT(Name)                \
  struct Name##Event : public CompanyEvent {       \
    explicit Name##Event(const Company* company) : \
      CompanyEvent(company) {}                     \
    ~Name##Event() override = default;             \
    DECLARE_EVENT_TYPE(CompanyEvent, Name);        \
  };
FOR_EACH_COMPANY_EVENT(DECLARE_COMPANY_EVENT);
#undef DECLARE_COMPANY_EVENT

DEFINE_EVENT_SUBJECT(Company);
DEFINE_EVENT_OBSERVABLE(Company);
FOR_EACH_COMPANY_EVENT(DEFINE_EVENT_OBSERVABLE);
}  // namespace prt

#endif  // PRT_COMPANY_EVENT_H
