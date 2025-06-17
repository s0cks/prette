#ifndef PRT_POPULATION_H
#define PRT_POPULATION_H

#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <string>
#include <vector>

#include "prette/citizen/citizen.h"
#include "prette/citizen/citizen_gen.h"
#include "prette/population_generated.h"  // IWYU pragma: export

namespace prt {
class City;
class Population {
  friend class City;
  friend class CityStorage;

 private:
  City* owner_;
  // citizens
  CitizenGenerator citizen_gen_;
  std::vector<Citizen> citizens_{};

  void Update(const TickDelta rhs);

  inline auto AllocNewCitizen() -> Citizen* {
    const auto idx = citizens_.size();
    citizens_.resize(citizens_.size() + 1);
    return &citizens_[idx];
  }

  void LoadFrom(const raw::Population& raw) {
    citizens_.reserve(raw.data()->size());
    for (auto idx = 0; idx < raw.data()->size(); idx++) {
      const auto value = raw.data()->Get(idx);
      citizens_.emplace_back(*value);
    }
  }

 public:
  explicit Population(City* owner);
  ~Population();

  auto GetOwner() const -> City* {
    return owner_;
  }

  auto IsEmpty() const -> bool {
    return citizens_.empty();
  }

  auto WriteTo(flatbuffers::FlatBufferBuilder& fbb) const -> flatbuffers::Offset<raw::Population>;
  auto CreateCitizen(const Gender gender, const std::string forename, const std::string surname) -> Citizen*;
  auto CreateCitizen(const Gender gender) -> Citizen*;
  auto CreateCitizen() -> Citizen*;

  inline auto CreateMaleCitizen(const std::string forename, const std::string surname) -> Citizen* {
    return CreateCitizen(Gender::kMale, forename, surname);
  }

  inline auto CreateMaleCitizen() -> Citizen* {
    return CreateCitizen(Gender::kMale);
  }

  inline auto CreateFemaleCitizen(const std::string forename, const std::string surname) -> Citizen* {
    return CreateCitizen(Gender::kMale, forename, surname);
  }

  inline auto CreateFemaleCitizen() -> Citizen* {
    return CreateCitizen(Gender::kMale);
  }
};
}  // namespace prt

#endif  // PRT_POPULATION_H
