#ifndef PRT_CITY_STORAGE_H
#define PRT_CITY_STORAGE_H

#include <fmt/format.h>
#include <functional>
#include <memory>
#include <rocksdb/db.h>
#include <string>

#include "prette/city/city.h"
#include "prette/city/population.h"
#include "prette/common.h"

namespace prt {
class CityStorage {
  friend class City;
  friend class CityManager;

 public:
  using DB = rocksdb::DB;

 private:
  fs::path root_;
  DB* index_ = nullptr;

  inline auto index() const -> DB* {
    return index_;
  }

  void LoadIndex();
  void SavePopulationTo(const fs::path& p, Population& population);
  void LoadPopulationFrom(const fs::path& p, Population& population);

 public:
  explicit CityStorage(const fs::path root);
  ~CityStorage();

  auto GetRoot() const -> const fs::path& {
    return root_;
  }

  auto GetIndexPath() const -> fs::path {
    return GetRoot() / "index";
  }

  auto GetPopulationDataPath(const City& rhs) const -> fs::path {
    return GetRoot() / fmt::format("{0:s}.dat", rhs.GetName());
  }

  auto Save(City& rhs) -> bool;
  auto Load(const std::string& name) -> std::unique_ptr<City>;
  auto VisitAllCities(std::function<bool(const City&)> vis) -> bool;
};
}  // namespace prt

#endif  // PRT_CITY_STORAGE_H
