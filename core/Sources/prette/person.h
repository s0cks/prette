#ifndef PRT_PERSON_H
#define PRT_PERSON_H

#include <concepts>
#include <functional>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/gender.h"
#include "prette/rx.h"

namespace prt {
class Person;
using PersonObservable = rx::observable<Person*>;
using PersonPredicate = std::function<bool(Person*)>;

template <typename T>
concept PersonPredicateLike = requires(T predicate, Person* person) {
  { predicate(person) } -> std::convertible_to<bool>;
};

class Person {
  friend class PersonGenerator;

 private:
  std::string first_name_{};
  std::string last_name_{};
  Gender gender_ = kMale;

 public:
  Person() = default;
  Person(const Gender gender, const std::string first_name, const std::string last_name) :
    gender_(gender),
    first_name_(std::move(first_name)),
    last_name_(std::move(last_name)) {
    ASSERT_NOT_EMPTY(first_name_);
    ASSERT_NOT_EMPTY(last_name_);
  }
  ~Person() = default;

  auto GetFirstName() const -> const std::string& {
    return first_name_;
  }

  auto GetLastName() const -> const std::string& {
    return last_name_;
  }

  auto GetGender() const -> Gender {
    return gender_;
  }

  inline auto IsMale() const -> bool {
    return GetGender() == Gender::kMale;
  }

  inline auto IsFemale() const -> bool {
    return GetGender() == kFemale;
  }

  auto ToString() const -> std::string;
};

class PersonBuilder {
 private:
  Gender gender_ = kMale;
  std::string first_name_{};
  std::string last_name_{};

 public:
  PersonBuilder() = default;
  ~PersonBuilder() = default;

  auto WithGender(const Gender rhs) -> PersonBuilder& {
    gender_ = rhs;
    return *this;
  }

  inline auto WithMaleGender() -> PersonBuilder& {
    return WithGender(Gender::kMale);
  }

  inline auto WithFemaleGender() -> PersonBuilder& {
    return WithGender(Gender::kFemale);
  }

  auto WithFirstName(const std::string& rhs) -> PersonBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    first_name_ = std::move(rhs);
    return *this;
  }

  auto WithLastName(const std::string& rhs) -> PersonBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    last_name_ = std::move(rhs);
    return *this;
  }

  auto IsValid() const -> bool {
    return !first_name_.empty() && !last_name_.empty();
  }

  auto Build() -> Person {
    ASSERT(IsValid());
    return {gender_, std::move(last_name_), std::move(first_name_)};
  }

  inline auto operator()() -> Person {
    return Build();
  }

  inline operator Person() {
    return Build();
  }
};
}  // namespace prt

#endif  // PRT_PERSON_H
