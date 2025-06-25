#ifndef PRT_WATER_H
#define PRT_WATER_H

#include <ostream>
#include <string>

namespace prt::water {
struct Sink {
  float per_tick;

  auto ToString() const -> std::string;

  inline friend auto operator<<(std::ostream& stream, const Sink& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};

struct Source {
  float per_tick;

  auto ToString() const -> std::string;

  inline friend auto operator<<(std::ostream& stream, const Source& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt::water

#endif  // PRT_WATER_H
