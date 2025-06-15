#ifndef PRT_CITY_H
#define PRT_CITY_H

#include <concepts>
#include <flatbuffers/flatbuffer_builder.h>
#include <functional>
#include <ostream>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/city_event.h"
#include "prette/city_generated.h"  // IWYU pragma: export
#include "prette/common.h"
#include "prette/tick.h"

namespace prt {
auto GetCityEventObservable() -> CityEventObservable;

template <typename... ArgN>
static inline auto OnCityEvent(ArgN... args) -> rx::composite_subscription {
  return GetCityEventObservable().subscribe(args...);
}

#define DEFINE_ONEVENT(Name)                                                            \
  static inline auto Get##Name##EventObservable()->Name##EventObservable {              \
    return GetCityEventObservable().filter(Name##Event::Filter).map(Name##Event::Cast); \
  }                                                                                     \
  template <typename... ArgN>                                                           \
  static inline auto On##Name##Event(ArgN... args)->rx::composite_subscription {        \
    return Get##Name##EventObservable().subscribe(args...);                             \
  }
FOR_EACH_CITY_EVENT(DEFINE_ONEVENT)
#undef DEFINE_EVENT

using CityId = int32_t;
static constexpr const auto kInvalidCityId = -1;

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
  DEFINE_DEFAULT_COPYABLE_TYPE(City);

 private:
  static void Publish(CityEvent* event);

  template <typename E, typename... Args>
  static inline void PublishCityEvent(Args... args) {
    E event(args...);
    return Publish(&event);
  }

#define DEFINE_PUBLISH(Name)                              \
  template <typename... Args>                             \
  static inline void Publish##Name##Event(Args... args) { \
    return PublishCityEvent<Name##Event>(args...);        \
  }
  FOR_EACH_CITY_EVENT(DEFINE_PUBLISH)
#undef DEFINE_PUBLISH

 private:
  CityId id_ = kInvalidCityId;
  std::string name_{};

  City(CityId id, const std::string name) :
    id_(id),
    name_(std::move(name)) {
    ASSERT_GTE(GetId(), 0);
    ASSERT(!name_.empty());
  }
  explicit City(const raw::City* raw_city);

  void Update(const TickDelta delta);

 public:
  City() = default;
  ~City() = default;

  auto GetId() const -> CityId {
    return id_;
  }

  auto GetName() const -> const std::string& {
    return name_;
  }

  void SaveTo(flatbuffers::FlatBufferBuilder& fbb) const;
  auto ToString() const -> std::string;

  inline auto CityId() const -> CityId {
    return GetId();
  }

  friend auto operator<<(std::ostream& stream, const City& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }

  auto operator<(const City& rhs) const -> bool {
    return GetId() < rhs.GetId();
  }
};
}  // namespace prt

#endif  // PRT_CITY_H
