#include <backward.hpp>
#include <cstdint>
#include <cstdlib>
#include <glog/logging.h>
#include <memory>
#include <random>
#include <string>
#include <units.h>
#include <utility>
#include <uuid.h>

#include "prette/assertions.h"
#include "prette/calendar_event.h"
#include "prette/city/city.h"
#include "prette/city/city_event.h"
#include "prette/city/city_manager.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/config/config_topic.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/hashcode.h"
#include "prette/lua.h"
#include "prette/registry.h"
#include "prette/rx.h"
#include "prette/settings/settings.h"
#include "prette/settings/settings_event.h"
#include "prette/to_string.h"

using std::toupper;

using namespace prt;

class TestItem {
 private:
  uuids::uuid id_;
  std::string name_;

 public:
  TestItem(const uuids::uuid& id, std::string name) :
    id_(std::move(id)),
    name_(std::move(name)) {}
  ~TestItem() = default;

  auto GetId() const -> uuids::uuid {
    return id_;
  }

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetHashCode() const -> HashCode {
    HashCode hash = 0;
    hash ^= name_;
    return hash;
  }

  auto Equals(const TestItem& rhs) const -> bool {
    return id_ == rhs.GetId() && name_ == rhs.GetName();
  }

  auto ToString() const -> std::string {
    ToStringHelper<TestItem> helper{};
    helper.AddFieldRef("id", GetId());
    helper.AddFieldRef("name", GetName());
    return helper;
  }
};

using TestItemRegistry = registry::Registry<TestItem>;

auto main(int argc, char** argv) -> int {
  Engine::Init(argc, argv);
  if (VLOG_IS_ON(1)) {
#define DEFINE_SUBSCRIBE_AND_LOG(Name) SUBSCRIBE_AND_LOG(GetConfig()->GetTopic().Get##Name##EventObservable(), INFO);
    FOR_EACH_CONFIG_TOPIC_EVENT(DEFINE_SUBSCRIBE_AND_LOG);
#undef DEFINE_SUBSCRIBE_AND_LOG

#define DEFINE_SUBSCRIBE_AND_LOG(Name) \
  SUBSCRIBE_AND_LOG(Settings::Get()->GetTopic().Get##Name##EventObservable(), INFO);
    FOR_EACH_SETTINGS_EVENT(DEFINE_SUBSCRIBE_AND_LOG);
#undef DEFINE_SUBSCRIBE_AND_LOG

    SUBSCRIBE_AND_LOG(GetLuaStateEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetNonTickEngineEventObservable(), INFO);
  }

  TestItemRegistry reg{};

  std::mt19937 random{};
  uuids::uuid_random_generator uuid_gen(random);
  const auto k1 = uuid_gen();
  const auto k2 = uuid_gen();
  const auto k3 = uuid_gen();

  LOG_IF(FATAL, !reg.Put(TestItem(k1, "Lacey"))) << "failed to insert v1";
  LOG_IF(FATAL, !reg.Put(TestItem(k3, "Test"))) << "failed to insert v2";

  LOG_IF(FATAL, !reg.Contains(k1)) << "failed to find v1 w/ key: " << k1;
  LOG_IF(FATAL, reg.Contains(k2)) << "found v2 w/ key: " << k2;
  LOG_IF(FATAL, !reg.Contains(k3)) << "failed to find v2 w/ key: " << k3;

  // OnPostInit([](PostInitEvent* event) {
  //   const auto manager = CityManager::Get();
  //   if (!manager->HasCity()) {
  //     if (!manager->LoadCity("Test")) {
  //       CityBuilder builder{};
  //       std::unique_ptr<City> city = builder.WithName("Test");
  //       manager->SetCity(std::move(city));
  //     }
  //   }
  //   ASSERT(manager->HasCity());
  //   const auto& city = manager->GetCity();
  //   ASSERT(city);
  //   DLOG(INFO) << "city: " << city->ToString();

  //   const auto& calendar = city->GetCalendar();
  //   SUBSCRIBE_AND_LOG(calendar.GetTopic().GetObservable<NewDayEvent>(), INFO);
  //   SUBSCRIBE_AND_LOG(calendar.GetTopic().GetObservable<NewMonthEvent>(), INFO);
  //   SUBSCRIBE_AND_LOG(calendar.GetTopic().GetObservable<NewYearEvent>(), INFO);
  //   SUBSCRIBE_AND_LOG(calendar.GetTopic().GetObservable<NewSeasonEvent>(), INFO);

  //   const auto duplex = city->NewDuplex(glm::fvec2(0.0f));
  //   const auto high_rise1 = city->NewHighRiseHousing(32, glm::fvec2(1.0f, 1.0f));
  //   const auto high_rise2 = city->NewHighRiseHousing(30, glm::fvec2(1.0f, 0.0f));
  //   const auto low_rise1 = city->NewLowRiseHousing(13, glm::fvec2(2.0f, 1.0f));
  //   const auto low_rise2 = city->NewLowRiseHousing(8, glm::fvec2(2.0f, 0.0f));
  //   DLOG(INFO) << "income: " << city->CalculateIncomePerTick().as_blocking().first();
  //   DLOG(INFO) << "expenses: " << city->CalculateExpensesPerTick().as_blocking().first();
  //   DLOG(INFO) << "water usage: " << city->CalculateWaterUsagePerTick().as_blocking().first();
  //   DLOG(INFO) << "water generation: " << city->CalculateWaterGenerationPerTick().as_blocking().first();
  //   DLOG(INFO) << "electricity usage: " << city->CalculateElectricityUsagePerTick().as_blocking().first();
  //   DLOG(INFO) << "electricity generation: " << city->CalculateElectricityGeneratePerTick().as_blocking().first();
  //   DLOG(INFO) << "housing units: " << city->GetNumberOfAvailableHousingUnits();

  //   // auto& population = city->GetPopulation();
  //   // if (population.IsEmpty()) {
  //   //   static constexpr const auto kMinCitizens = 2;
  //   //   static constexpr const auto kMaxCitizens = 50;
  //   //   std::mt19937 random(std::random_device{}());
  //   //   std::uniform_int_distribution<int> distribution(kMinCitizens, kMaxCitizens);
  //   //   const auto num_citizens = distribution(random);
  //   //   DLOG(INFO) << "creating " << num_citizens << " citizens....";
  //   //   for (auto idx = 0; idx < num_citizens; idx++) {
  //   //     const auto new_citizen = population.CreateCitizen();
  //   //     ASSERT(new_citizen);
  //   //     DLOG(INFO) << "created: " << new_citizen->ToString();
  //   //   }
  //   //   CityManager::Get()->SaveCity();
  //   // }
  // });

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
