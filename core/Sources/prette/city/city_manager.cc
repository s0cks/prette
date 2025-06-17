#include "prette/city/city_manager.h"

#include <glog/logging.h>
#include <marl/defer.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>
#include <memory>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/city/city.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/flags.h"
#include "prette/thread_local.h"
#include "prette/uv/loop.h"

namespace prt {
static ThreadLocal<CityManager> manager_{};

CityManager::CityManager(uv::Loop& loop, const fs::path root) :
  storage_(root),
  topic_(),
  autosave_timer_(loop, [this] {
    return SaveCity(SaveReason::kAuto);
  }) {}

CityManager::~CityManager() {
  StopAutosaveTimer();
  autosave_timer_.Close();
}

void CityManager::Init() {
  ASSERT(!IsInitialized());
  manager_ = new CityManager(GetEngine()->GetLoop(), fs::path(FLAGS_resources) / "cities");
}

auto CityManager::IsInitialized() -> bool {
  return manager_.Get() != nullptr;
}

auto CityManager::Get() -> CityManager* {
  ASSERT(IsInitialized());
  return manager_;
}

auto CityManager::SaveCity(const SaveReason reason) -> bool {
  if (!HasCity()) {
    LOG(WARNING) << "no city to save.";
    return false;
  }

  if (!storage_.Save(*city_)) {
    LOG(ERROR) << "failed to save current city: " << GetCity()->ToString();
    return false;
  }

  GetCity()->GetTopic().PublishCitySavedEvent();
  return true;
}

auto CityManager::LoadCity(const std::string name) -> bool {
  ASSERT_NOT_EMPTY(name);
  City* previous = nullptr;
  if (HasCity()) {
    NOT_IMPLEMENTED(ERROR);  // TODO: @s0cks implement
    LOG(ERROR) << "cannot load another city.";
    return false;
  }
  ASSERT(!HasCity());
  if (!(city_ = storage_.Load(name))) {
    LOG(ERROR) << "failed to load City named: " << name;
    return false;
  }
  ASSERT(HasCity());
  GetCity()->GetTopic().PublishCityLoadedEvent();
  topic_.PublishCityChangedEvent(previous, city_.get());
  StartAutosaveTimer();
  return true;
}

void CityManager::SetCity(std::unique_ptr<City> rhs) {
  ASSERT(rhs);
  StopAutosaveTimer();
  if (HasCity())
    SaveCity(SaveReason::kDeInit);
  std::swap(rhs, city_);
  topic_.PublishCityChangedEvent(rhs.get(), city_.get());
  SaveCity(SaveReason::kInit);
  StartAutosaveTimer();
}
}  // namespace prt