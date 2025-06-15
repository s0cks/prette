#ifndef PRT_CITY_STORAGE_H
#define PRT_CITY_STORAGE_H

#include <vector>

#include "prette/city.h"
#include "prette/common.h"

namespace prt {
class CityStorage {
  friend class City;
  friend class CityManager;

 private:
  fs::path root_;

  void SaveCity(const City& rhs) const;
  void SaveAllCities(const std::vector<City>& cities) const;
  auto LoadAllCities(std::vector<City>& results) -> bool;

 private:
  static auto LoadCityFrom(const fs::path& p, City* result) -> bool;
  static auto SaveCityTo(const City& city, const fs::path& p) -> bool;

 public:
  explicit CityStorage(const fs::path root);
  ~CityStorage();

  auto GetRoot() const -> const fs::path& {
    return root_;
  }
};
}  // namespace prt

#endif  // PRT_CITY_STORAGE_H
