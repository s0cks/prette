#include "prette/city/city.h"

#include <chrono>
#include <entt/entity/fwd.hpp>
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
#include "prette/city/building.h"
#include "prette/city/city_event.h"
#include "prette/city/housing.h"
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

auto City::NewHousing(const HousingType type, const uint64_t num_units, const glm::fvec2 pos) -> entt::entity {
  const auto entity = registry_.create();
  registry_.emplace<Housing>(entity, type, num_units);
  registry_.emplace<Building>(entity, std::move(pos));
  return entity;
}

void City::Update(const TickDelta delta) {
  calendar_.Update(delta);
  // CalculateAttractiveness();
  population_.Update(delta);
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