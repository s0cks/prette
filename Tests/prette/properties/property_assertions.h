#ifndef PRT_PROPERTY_ASSERTIONS_H
#define PRT_PROPERTY_ASSERTIONS_H

#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "prette/properties/property.h"

namespace prt::properties {
  using namespace ::testing;

  static inline auto
  HasName(Property* p, const std::string& name) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(p->GetName() != name)
      return AssertionFailure() << "expected " << p->ToString() << " to have name: " << name;
    return AssertionSuccess();
  }

  static inline auto
  HasValue(Property* p, const std::string& value) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsString())
      return AssertionFailure() << "expected " << p->ToString() << " to be a String property.";
    if(p->AsString()->GetValue() != value)
      return AssertionFailure() << "expected " << p->ToString() << " to have value: " << value;
    return AssertionSuccess();
  }

  static inline auto
  HasValue(Property* p, const uint64_t value) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsLong())
      return AssertionFailure() << "expected " << p->ToString() << " to be a Long property.";
    if(p->AsLong()->GetValue() != value)
      return AssertionFailure() << "expected " << p->ToString() << " to have value: " << value;
    return AssertionSuccess();
  }

  static inline auto
  HasValue(Property* p, const double value) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsDouble())
      return AssertionFailure() << "expected " << p->ToString() << " to be a Double property.";
    if(p->AsDouble()->GetValue() != value)
      return AssertionFailure() << "expected " << p->ToString() << " to have value: " << value;
    return AssertionSuccess();
  }

  static inline auto
  HasValue(Property* p, const bool value) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsBool())
      return AssertionFailure() << "expected " << p->ToString() << " to be a Bool property.";
    if(p->AsBool()->GetValue() != value)
      return AssertionFailure() << "expected " << p->ToString() << " to have value: " << value;
    return AssertionSuccess();
  }

#define DEFINE_TYPE_CHECK(Name)                                                                           \
  static inline auto                                                                           \
  Is##Name##Property(Property* p) -> AssertionResult {                                                                       \
    if(!p)                                                                                                \
      return AssertionFailure() << "expected value to not be null.";                                      \
    return p->Is##Name()                                                                                  \
         ? AssertionSuccess()                                                                             \
         : AssertionFailure() << "expected " << p->ToString() << " to be a " << #Name << " property.";    \
  }
  FOR_EACH_PROPERTY_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

  static inline auto
  IsTrue(Property* p) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsBool())
      return AssertionFailure() << "expected " << p->ToString() << " to be a Bool.";
    if(!*p->AsBool())
      return AssertionFailure() << "expected " << p->ToString() << " to be True.";
    return AssertionSuccess();
  }

  static inline auto
  IsFalse(Property* p) -> AssertionResult {
    if(!p)
      return AssertionFailure() << "expected value to not be null.";
    if(!p->IsBool())
      return AssertionFailure() << "expected " << p->ToString() << " to be a Bool.";
    if(*p->AsBool())
      return AssertionFailure() << "expected " << p->ToString() << " to be False.";
    return AssertionSuccess();
  }
}

#endif //PRT_PROPERTY_ASSERTIONS_H