#ifndef PRT_CITY_H
#define PRT_CITY_H

#include <concepts>
#include <flatbuffers/flatbuffer_builder.h>
#include <functional>
#include <memory>
#include <ostream>
#include <random>
#include <rocksdb/slice.h>
#include <string>
#include <utility>
#include <uv.h>

#include "prette/assertions.h"
#include "prette/city/citizen.h"
#include "prette/city/city_event.h"
#include "prette/city/city_topic.h"
#include "prette/city/population.h"
#include "prette/city_generated.h"  // IWYU pragma: export
#include "prette/common.h"
#include "prette/relaxed_atomic.h"
#include "prette/tick.h"

namespace prt {
class City;
using CityObservable = rx::observable<City*>;
using CityPredicate = std::function<bool(City*)>;

template <typename Predicate>
concept CityPredicateLike = requires(Predicate p, City* value) {
  { p(value) } -> std::convertible_to<bool>;
};

class City {
  friend class CityStorage;
  friend class CityManager;
  DEFINE_NON_COPYABLE_TYPE(City);

 public:
  static constexpr const auto kDefaultInitAttractiveness = 1.0f;
  static constexpr const auto kMaxNumberOfImmigrantsPerTick = 128;
  static constexpr const auto kMinNumberOfImmigrantsPerTick = 2;

 private:
  std::string name_{};
  uint64_t seed_{};
  std::mt19937 random_;
  CityTopic topic_;
  rx::subscription on_tick_{};
  Population population_;
  RelaxedAtomic<uword> available_jobs_{};
  RelaxedAtomic<uword> available_houses_{};
  float attractiveness_ = kDefaultInitAttractiveness;
  uint8_t hour_ = 0;
  uint8_t day_ = 0;
  uint8_t month_ = 0;
  uint8_t year_ = 0;

  void UpdateTime();
  void Update(const TickDelta delta);
  void UpdateCitizens(const TickDelta rhs);
  void CalculateAttractiveness();
  void CalculateTransplants();
  void SetPopulation(const Population& rhs);

  inline auto topic() -> CityTopic& {
    return topic_;
  }

 public:
  explicit City(const std::string name, const uint64_t seed = 0);
  explicit City(const raw::City* raw) :
    City(raw->name()->str(), raw->seed()) {}
  explicit City(const rocksdb::Slice& slice) :
    City(raw::GetCity(slice.data())) {}
  ~City();

  auto GetSeed() const -> uint64_t {
    return seed_;
  }

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetTopic() -> CityTopic& {
    return topic_;
  }

  auto GetTopic() const -> const CityTopic& {
    return topic_;
  }

  auto GetPopulation() -> Population& {
    return population_;
  }

  auto GetPopulation() const -> const Population& {
    return population_;
  }

  void SaveTo(flatbuffers::FlatBufferBuilder& fbb) const;
  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const City& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }

  inline auto operator>>(flatbuffers::FlatBufferBuilder& fbb) const -> flatbuffers::FlatBufferBuilder& {
    SaveTo(fbb);
    return fbb;
  }
};

class CityBuilder {
 private:
  std::string name_{};
  uint64_t seed_{};

 public:
  CityBuilder() = default;
  ~CityBuilder() = default;

  auto WithName(const std::string rhs) -> CityBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    name_ = std::move(rhs);
    return *this;
  }

  auto WithName(const uint64_t rhs) -> CityBuilder& {
    ASSERT_GT(rhs, 0);
    seed_ = rhs;
    return *this;
  }

  auto IsValid() const -> bool {
    return !name_.empty();
  }

  auto Build() -> std::unique_ptr<City>;

  inline auto operator()() -> std::unique_ptr<City> {
    return Build();
  }

  inline operator std::unique_ptr<City>() {
    return Build();
  }
};

static inline auto operator<<(std::ostream& stream, const City* rhs) -> std::ostream& {
  return stream << (rhs ? rhs->ToString() : "null");
}
}  // namespace prt

#endif  // PRT_CITY_H
