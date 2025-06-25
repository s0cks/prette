#ifndef PRT_CITY_H
#define PRT_CITY_H

#include <concepts>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
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
#include "prette/calendar.h"
#include "prette/city/citizen.h"
#include "prette/city/city_event.h"
#include "prette/city/city_topic.h"
#include "prette/city/electricity.h"
#include "prette/city/housing.h"
#include "prette/city/income.h"
#include "prette/city/population.h"
#include "prette/city/water.h"
#include "prette/city_generated.h"  // IWYU pragma: export
#include "prette/common.h"
#include "prette/glm.h"
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

template <typename Component>
concept PerTickComponent = requires(Component c) {
  { c.per_tick } -> std::convertible_to<float>;
};

template <PerTickComponent C>
static inline constexpr auto GetPerTick() -> std::function<float(const C*)> {
  return [](const C* value) {
    return value->per_tick;
  };
}

class City {
  friend class CityStorage;
  friend class CityManager;
  DEFINE_NON_COPYABLE_TYPE(City);

 public:
  static constexpr const auto kDefaultInitAttractiveness = 1.0f;
  static constexpr const auto kMaxNumberOfImmigrantsPerTick = 128;
  static constexpr const auto kMinNumberOfImmigrantsPerTick = 2;

 private:
  entt::registry registry_{};
  std::string name_{};
  uint64_t seed_{};
  std::mt19937 random_;
  CityTopic topic_;
  rx::subscription on_tick_{};
  Population population_;
  RelaxedAtomic<uword> available_jobs_{};
  RelaxedAtomic<uword> available_houses_{};
  float attractiveness_ = kDefaultInitAttractiveness;
  Calendar calendar_{};

  void Update(const TickDelta delta);
  void UpdateCitizens(const TickDelta rhs);
  void SetPopulation(const Population& rhs);

  template <typename C>
  inline auto GetComponentObservable() const -> rx::observable<const C*> {
    return rx::observable<>::create<const C*>([this](rx::subscriber<const C*> s) {
      const auto view = registry_.view<const C>();
      for (const auto [entity, c] : view.each())
        s.on_next(&c);
      s.on_completed();
    });
  }

  template <PerTickComponent C>
  inline auto CalculateSumPerTick() const -> rx::observable<float> {
    // clang-format off
    return GetComponentObservable<C>()
      .map(GetPerTick<C>())
      .reduce(0.0f, [](const float sum, const float value) {
        return sum + value;
      });
    // clang-format on
  }

  auto NewHousing(const HousingType type, const uint64_t num_units, const glm::fvec2 pos) -> entt::entity;

  template <PerTickComponent E>
  inline auto AddPerTickComponent(const entt::entity& e, const float per_tick) -> E& {
    return registry_.emplace<E>(e, per_tick);
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

  auto GetRegistry() -> entt::registry& {
    return registry_;
  }

  auto GetRegistry() const -> const entt::registry& {
    return registry_;
  }

  auto GetCalendar() const -> const Calendar& {
    return calendar_;
  }

  auto GetCalendar() -> Calendar& {
    return calendar_;
  }

  inline auto NewSingleFamily(const glm::fvec2 pos) -> entt::entity {
    return NewHousing(HousingType::kSingleFamilyHousing, 1, std::move(pos));
  }

  inline auto NewDuplex(const glm::fvec2 pos) -> entt::entity {
    return NewHousing(HousingType::kDuplexHousing, 2, std::move(pos));
  }

  inline auto NewLowRiseHousing(const uint64_t num_units, const glm::fvec2 pos) -> entt::entity {
    return NewHousing(HousingType::kLowRiseHousing, num_units, std::move(pos));
  }

  inline auto NewHighRiseHousing(const uint64_t num_units, const glm::fvec2 pos) -> entt::entity {
    return NewHousing(HousingType::kHighRiseHousing, num_units, std::move(pos));
  }

  inline auto CalculateIncomePerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<income::Source>();
  }

  inline auto CalculateExpensesPerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<income::Sink>();
  }

  inline auto CalculateElectricityGeneratePerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<electricity::Source>();
  }

  inline auto CalculateElectricityUsagePerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<electricity::Sink>();
  }

  inline auto CalculateWaterGenerationPerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<water::Source>();
  }

  inline auto CalculateWaterUsagePerTick() const -> rx::observable<float> {
    return CalculateSumPerTick<water::Sink>();
  }

  inline auto GetNumberOfAvailableHousingUnits() const -> uint64_t {
    // clang-format off
    return GetComponentObservable<Housing>()
        .map([](const Housing* value) -> uword {return value->available_units; })
        .as_blocking()
        .sum();
    // clang-format on
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
