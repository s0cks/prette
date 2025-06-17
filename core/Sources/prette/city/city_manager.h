#ifndef PRT_CITY_MANAGER_H
#define PRT_CITY_MANAGER_H

#include <filesystem>
#include <marl/waitgroup.h>
#include <memory>
#include <string>
#include <units.h>

#include "prette/city/city.h"
#include "prette/city/city_manager_topic.h"
#include "prette/city/city_storage.h"
#include "prette/common.h"
#include "prette/uv/loop.h"
#include "prette/uv/timer.h"

namespace prt {
class CityManager {
  friend class City;
  DEFINE_NON_COPYABLE_TYPE(CityManager);

 public:
  static constexpr const units::time::millisecond_t kAutosaveTimerTimeout = units::time::millisecond_t(0);
  static constexpr const units::time::millisecond_t kAutosaveTimerInterval = units::time::second_t(30);

  enum class SaveReason : uint8_t {
    kNone = 0,
    kInit,
    kDeInit,
    kManual,
    kAuto,
  };

 private:
  CityStorage storage_;
  CityManagerTopic topic_;
  uv::Timer autosave_timer_;
  std::unique_ptr<City> city_{};

  CityManager(uv::Loop& loop, const fs::path root);

  inline void StartAutosaveTimer(const uint64_t timeout = kAutosaveTimerTimeout.value(),
                                 const uint64_t repeat = kAutosaveTimerInterval.value()) {
    return autosave_timer_.Start(timeout, repeat);
  }

  inline void StopAutosaveTimer() {
    return autosave_timer_.Stop();
  }

  auto SaveCity(const SaveReason reason) -> bool;

 public:
  ~CityManager();

  auto GetStorage() const -> const CityStorage& {
    return storage_;
  }

  auto GetTopic() const -> const CityManagerTopic& {
    return topic_;
  }

  auto GetCity() const -> const std::unique_ptr<City>& {
    return city_;
  }

  inline auto HasCity() const -> bool {
    return GetCity() != nullptr;
  }

  inline auto SaveCity() -> bool {
    return SaveCity(SaveReason::kManual);
  }

  auto LoadCity(const std::string name) -> bool;
  void SetCity(std::unique_ptr<City> rhs);

 public:
  static void Init();
  static auto IsInitialized() -> bool;
  static auto Get() -> CityManager*;
};
}  // namespace prt

#endif  // PRT_CITY_MANAGER_H
