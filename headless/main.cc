#include <backward.hpp>
#include <cstdlib>
#include <glog/logging.h>
#include <units.h>

#include "prette/assertions.h"
#include "prette/city.h"
#include "prette/city_manager.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/gender.h"
#include "prette/lua.h"
#include "prette/name_gen.h"
#include "prette/rx.h"
#include "prette/settings/settings.h"
#include "prette/world/world.h"

using namespace prt;

auto main(int argc, char** argv) -> int {
  Engine::Init(argc, argv);
  if (VLOG_IS_ON(1)) {
    SUBSCRIBE_AND_LOG(GetConfigEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetSettingsEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetLuaStateEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetWorldEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetNonTickEngineEventObservable(), INFO);
  }

  FullnameGenerator gen(Gender::kFemale);
  const auto [forename, surname] = gen();
  DLOG(INFO) << "generated: " << forename << " " << surname;

  OnPostInit([](PostInitEvent* event) {
    const auto manager = CityManager::Get();
    if (manager->GetNumberOfCities() == 0) {
      const auto new_city = CityManager::Get()->NewCity("Test");
    } else {
      ASSERT_GT(manager->GetNumberOfCities(), 0);
      const auto city = CityManager::Get()->GetCityAt(0);
      const auto person1 = CityManager::Get()->NewCitizen(0, Gender::kMale);
      DLOG(INFO) << person1->ToString() << " created!";

      const auto person2 = CityManager::Get()->NewCitizen(0, Gender::kFemale);
      DLOG(INFO) << person2->ToString() << " created!";
    }
  });

  const auto engine = GetEngine();
  ASSERT(engine);
  return engine->Run();
}