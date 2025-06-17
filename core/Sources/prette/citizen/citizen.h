#ifndef PRT_CITIZEN_H
#define PRT_CITIZEN_H

#include <array>
#include <concepts>
#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <functional>
#include <ostream>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/rx.h"

// IWYU pragma: begin_exports
#include "prette/citizen/gender.h"
#include "prette/citizen_generated.h"
#include "prette/needs.h"
#include "prette/tick.h"
// IWYU pragma: end_exports

namespace prt {
class Citizen;
using CitizenObservable = rx::observable<Citizen*>;
using CitizenPredicate = std::function<bool(Citizen*)>;

template <typename T>
concept CitizenPredicateLike = requires(T predicate, Citizen* citizen) {
  { predicate(citizen) } -> std::convertible_to<bool>;
};

class Citizen {
  friend class City;
  friend class Population;
  friend class CitizenGenerator;

 public:
  using NeedsData = std::array<float, kTotalNumberOfNeeds>;

 private:
  Gender gender_ = kMale;
  std::string forename_{};
  std::string surname_{};
  NeedsData needs_{};

  void Update(const TickDelta delta);

 public:
  Citizen() = default;
  Citizen(const Gender gender, const std::string forename, const std::string surname);
  explicit Citizen(const raw::Citizen& raw) :
    Citizen(static_cast<Gender>(raw.gender()), raw.forename()->str(), raw.surname()->str()) {}
  ~Citizen();

  auto GetForename() const -> const std::string& {
    return forename_;
  }

  auto GetSurname() const -> const std::string& {
    return surname_;
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
  auto WriteTo(flatbuffers::FlatBufferBuilder& fbb) const -> flatbuffers::Offset<raw::Citizen>;

  friend auto operator<<(std::ostream& stream, const Citizen& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }

  auto operator>>(flatbuffers::FlatBufferBuilder& fbb) const -> flatbuffers::FlatBufferBuilder& {
    WriteTo(fbb);
    return fbb;
  }
};

class CitizenBuilder {
 private:
  Gender gender_ = kMale;
  std::string forename_{};
  std::string surname_{};

 public:
  CitizenBuilder() = default;
  ~CitizenBuilder() = default;

  auto WithGender(const Gender rhs) -> CitizenBuilder& {
    gender_ = rhs;
    return *this;
  }

  inline auto WithMaleGender() -> CitizenBuilder& {
    return WithGender(Gender::kMale);
  }

  inline auto WithFemaleGender() -> CitizenBuilder& {
    return WithGender(Gender::kFemale);
  }

  auto WithForename(const std::string& rhs) -> CitizenBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    forename_ = std::move(rhs);
    return *this;
  }

  auto WithSurname(const std::string& rhs) -> CitizenBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    surname_ = std::move(rhs);
    return *this;
  }

  auto IsValid() const -> bool {
    return !forename_.empty() && !surname_.empty();
  }

  auto Build() -> Citizen {
    ASSERT(IsValid());
    return {gender_, std::move(forename_), std::move(surname_)};
  }

  inline auto operator()() -> Citizen {
    return Build();
  }

  inline operator Citizen() {
    return Build();
  }
};
}  // namespace prt

#endif  // PRT_CITIZEN_H
