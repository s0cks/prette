#include "prette/city_manager.h"

#include <algorithm>
#include <glog/logging.h>
#include <marl/defer.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>
#include <string>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/city.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/flags.h"
#include "prette/gender.h"
#include "prette/name_gen.h"
#include "prette/person.h"
#include "prette/rx.h"
#include "prette/thread_local.h"
#include "prette/tick.h"
#include "prette/to_string.h"

namespace prt {
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    return ToStringHelper<Name##Event>{};           \
  }
FOR_EACH_CITY_MANAGER_EVENT(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING

static CityManagerEventSubject events_{};
static ThreadLocal<CityManager> manager_{};

auto GetCityManagerEventObservable() -> CityManagerEventObservable {
  return events_.get_observable();
}

void CityManager::Publish(CityManagerEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

CityManager::CityManager(const fs::path root) :
  storage_(root) {
  LoadAllCities();
}

CityManager::~CityManager() {
  on_tick_.unsubscribe();
}

void CityManager::LoadAllCities() {
  on_tick_.unsubscribe();
  DLOG(INFO) << "loading cities....";
  if (!cities_.empty())
    cities_.clear();
  storage_.LoadAllCities(cities_);
  std::ranges::sort(cities_, [](const City& lhs, const City& rhs) {
    return lhs < rhs;
  });
  citizens_.resize(cities_.size());
  for (auto& citizens : citizens_)
    citizens.reserve(1024);
  PublishCitiesLoadedEvent();
  on_tick_ = OnTick([this](TickEvent* event) {
    UpdateCities(event->GetTimeSinceLast());
  });
}

void CityManager::SaveAllCities() {
  DLOG(INFO) << "saving cities....";
  if (!cities_.empty())
    return;
  storage_.SaveAllCities(cities_);
  return PublishCitiesSavedEvent();
}

void CityManager::UpdateCities(const TickDelta delta) {
  PRT_PROFILING_BEGIN(update_all);
  const CityPredicate update = [this, delta](City* city) {
    city->Update(delta);
    return true;
  };
  LOG_IF(FATAL, !VisitAllCities(update)) << "failed to update cities.";
  PRT_PROFILING_END(update_all);
  LOG_IF_EVERY_N(INFO, VLOG_IS_ON(1), 100000)
      << "updated " << GetNumberOfCities() << " cities in " << (update_all_duration).count() << "ns.";
  PublishCitiesUpdatedEvent();
}

void CityManager::Init() {
  ASSERT(!IsInitialized());
  manager_ = new CityManager(fs::path(FLAGS_resources) / "cities");
}

auto CityManager::IsInitialized() -> bool {
  return manager_.Get() != nullptr;
}

auto CityManager::Get() -> CityManager* {
  ASSERT(IsInitialized());
  return manager_;
}

auto CityManager::NewCity(const std::string name) -> City* {
  ASSERT(!name.empty());
  const auto idx = cities_.size();
  cities_.resize(idx + 1);
  citizens_.resize(idx + 1);
  citizens_[idx].reserve(1024);
  auto city = new (&cities_[idx]) City(static_cast<CityId>(idx), std::move(name));
  City::PublishCityCreatedEvent(city);
  storage_.SaveCity((*city));
  return city;
}

auto CityManager::NewCitizen(const CityId id, const Gender gender, const std::string forename,
                             const std::string surname) -> Person* {
  auto& city_citizens = citizens(id);
  const auto idx = city_citizens.size();
  city_citizens.resize(idx + 1);
  const auto person = new (&city_citizens[idx]) Person(gender, forename, surname);
  return person;
}

auto CityManager::NewCitizen(const CityId id, const Gender gender) -> Person* {
  FullnameGenerator gen(gender);
  const auto [forename, surname] = gen();
  return NewCitizen(id, gender, forename, surname);
}
}  // namespace prt