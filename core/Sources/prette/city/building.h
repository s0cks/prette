#ifndef PRT_BUILDING_H
#define PRT_BUILDING_H

#include <ostream>
#include <string>

#include "prette/glm.h"

namespace prt {
struct Building {
  glm::fvec2 pos{};

  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const Building& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt

#endif  // PRT_BUILDING_H
