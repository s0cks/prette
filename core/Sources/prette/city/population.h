#ifndef PRT_POPULATION_H
#define PRT_POPULATION_H

#include <cstdint>
#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <string>
#include <vector>

#include "prette/assertions.h"
#include "prette/city/citizen.h"
#include "prette/city/citizen_gen.h"
#include "prette/population_generated.h"  // IWYU pragma: export

namespace prt {
class City;
class Population {
  friend class City;
  friend class CityStorage;

 public:
  class Iterator {
   private:
    Population* population_;
    uint64_t current_ = 0;

   public:
    explicit Iterator(Population* population) :
      population_(population) {
      ASSERT(population_);
    }
    ~Iterator() = default;

    auto GetPopulation() const -> Population* {
      return population_;
    }

    auto HasNext() const -> bool {
      return current_ < GetPopulation()->GetLength();
    }

    auto Next() -> Citizen* {
      const auto next = GetPopulation()->GetCitizenAt(current_);
      current_ += 1;
      return next;
    }
  };

 private:
  City* owner_;
  // citizens
  CitizenGenerator citizen_gen_;
  std::vector<Citizen> citizens_{};

  inline auto AllocNewCitizen() -> Citizen* {  // TODO: check bounds
    const auto idx = citizens_.size();
    citizens_.resize(idx + 1);
    return &citizens_[idx];
  }

  void Update(const TickDelta rhs);
  void LoadFrom(const raw::Population& raw);

 public:
  explicit Population(City* owner, const uint64_t init_cap = 128);
  ~Population();

  auto GetOwner() const -> City* {
    return owner_;
  }

  auto GetLength() const -> uint64_t {
    return citizens_.size();
  }

  inline auto GetCitizenAt(const uint64_t idx) -> Citizen* {
    return &citizens_.at(idx);
  }

  inline auto VisitAllCitizens(CitizenVisitor* vis) -> bool {
    ASSERT(vis);
    Iterator iter(this);
    while (iter.HasNext()) {
      const auto next = iter.Next();
      if (!vis->Visit(next))
        return false;
    }
    return true;
  }

  inline auto VisitAllCitizens(const CitizenPredicate& vis) -> bool {
    Iterator iter(this);
    while (iter.HasNext()) {
      const auto next = iter.Next();
      if (!vis(next))
        return false;
    }
    return true;
  }

  auto WriteTo(flatbuffers::FlatBufferBuilder& fbb) -> flatbuffers::Offset<raw::Population>;
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
