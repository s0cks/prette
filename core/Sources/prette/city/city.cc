#include "prette/city/city.h"

#include <chrono>
#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <marl/defer.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>
#include <memory>
#include <random>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/city/city_event.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/event.h"
#include "prette/tick.h"
#include "prette/to_string.h"

namespace prt {
DEFINE_GLOBAL_EVENT_SUBJECT(CityEvent, events_);

static inline auto ProtectSeed(const uint64_t seed) -> uint64_t {
  if (seed != 0) {
    return seed;
  } else [[likely]] {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }
}

City::City(const std::string name, const uint64_t seed) :
  name_(std::move(name)),
  seed_(ProtectSeed(seed)),
  random_(GetSeed()),
  topic_(this),
  population_(this) {
  on_tick_ = OnTick([this](TickEvent* event) {
    return Update(event->GetTimeSinceLast());
  });
}

City::~City() {
  on_tick_.unsubscribe();
}

void City::CalculateAttractiveness() {
  attractiveness_ = 0.1f;
}

void City::CalculateTransplants() {
  static std::uniform_int_distribution<uint32_t> distribution(kMinNumberOfImmigrantsPerTick,
                                                              kMaxNumberOfImmigrantsPerTick);
  auto num_immigrants = static_cast<uint32_t>(distribution(random_));
  num_immigrants = static_cast<uint32_t>(round(static_cast<float>(num_immigrants) * attractiveness_));
  DVLOG(2) << "creating " << num_immigrants << " immigrants....";
  for (auto idx = 0; idx < num_immigrants; idx++) {
    auto new_person = GetPopulation().CreateCitizen();
    ASSERT(new_person);
  }
}

void City::UpdateTime() {
  hour_ += 1;
  if (hour_ >= 24) {
    hour_ = 0;
    day_ += 1;
    if ((month_ == 2 && day_ >= 27) || day_ >= 30) {
      day_ = 0;
      topic_.PublishNewDayEvent();
      month_ += 1;
      topic_.PublishNewMonthEvent();
      if (month_ >= 12) {
        month_ = 0;
        year_ += 1;
        topic_.PublishNewYearEvent();
      } else {
        topic_.PublishNewMonthEvent();
      }
    } else {
      topic_.PublishNewDayEvent();
    }
  }
}

void City::Update(const TickDelta delta) {
  UpdateTime();
  // CalculateAttractiveness();
  population_.Update(delta);
  CalculateTransplants();
  topic_.PublishCityUpdatedEvent();
}

void City::SaveTo(flatbuffers::FlatBufferBuilder& fbb) const {
  const auto name_offset = fbb.CreateString(GetName());
  raw::CityBuilder builder(fbb);
  builder.add_name(name_offset);
  const auto raw_city = builder.Finish();
  return fbb.Finish(raw_city);
}

auto City::ToString() const -> std::string {
  ToStringHelper<City> helper{};
  helper.AddField("name", GetName());
  return helper;
}

auto CityBuilder::Build() -> std::unique_ptr<City> {
  ASSERT(IsValid());
  return std::make_unique<City>(name_, seed_);
}
}  // namespace prt