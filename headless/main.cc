#include <backward.hpp>
#include <cstdlib>
#include <glog/logging.h>
#include <memory>
#include <units.h>
#include <utility>

#include "prette/assertions.h"
#include "prette/city/city.h"
#include "prette/city/city_event.h"
#include "prette/city/city_manager.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/lua.h"
#include "prette/rx.h"
#include "prette/settings/settings.h"
#include "prette/world/world.h"

using std::toupper;

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

  OnPostInit([](PostInitEvent* event) {
    const auto manager = CityManager::Get();
    if (!manager->HasCity()) {
      if (!manager->LoadCity("Test")) {
        CityBuilder builder{};
        std::unique_ptr<City> city = builder.WithName("Test");
        manager->SetCity(std::move(city));
      }
    }
    ASSERT(manager->HasCity());
    const auto& city = manager->GetCity();
    ASSERT(city);
    DLOG(INFO) << "city: " << city->ToString();
    SUBSCRIBE_AND_LOG(city->GetTopic().GetNewDayEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(city->GetTopic().GetNewMonthEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(city->GetTopic().GetNewYearEventObservable(), INFO);
    // auto& population = city->GetPopulation();
    // if (population.IsEmpty()) {
    //   static constexpr const auto kMinCitizens = 2;
    //   static constexpr const auto kMaxCitizens = 50;
    //   std::mt19937 random(std::random_device{}());
    //   std::uniform_int_distribution<int> distribution(kMinCitizens, kMaxCitizens);
    //   const auto num_citizens = distribution(random);
    //   DLOG(INFO) << "creating " << num_citizens << " citizens....";
    //   for (auto idx = 0; idx < num_citizens; idx++) {
    //     const auto new_citizen = population.CreateCitizen();
    //     ASSERT(new_citizen);
    //     DLOG(INFO) << "created: " << new_citizen->ToString();
    //   }
    //   CityManager::Get()->SaveCity();
    // }
  });

  const auto engine = GetEngine();
  ASSERT(engine);
  return engine->Run();
}

// pipe::source<char> hello_world = [raw = "Hello World"](pipe::sink<char> s) {
//   for (auto it = raw; *it; it++)
//     s(*it);
// };
// pipe::phase<char, char> to_upper = [](pipe::source<char> in, pipe::sink<char> out) {
//   return in([&out](char c) {
//     return out(static_cast<char>(toupper(c)));
//   });
// };
// pipe::sink<char> print = [](char c) {
//   std::cout << c;
// };

// const auto p = hello_world | to_upper | print;
// p();
