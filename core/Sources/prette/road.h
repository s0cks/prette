#ifndef PRT_ROAD_H
#define PRT_ROAD_H

#include <string>
#include <utility>

#include "prette/common.h"

namespace prt {
class Road {
  DEFINE_DEFAULT_COPYABLE_TYPE(Road);

 private:
  std::string name_;

 public:
  explicit Road(const std::string name) :
    name_(std::move(name)) {}
  ~Road() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto ToString() const -> std::string;
};
}  // namespace prt

#endif  // PRT_ROAD_H
