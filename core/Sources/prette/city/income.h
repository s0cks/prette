#ifndef PRT_INCOME_H
#define PRT_INCOME_H

#include <ostream>
#include <string>

namespace prt::income {
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
}  // namespace prt::income

#endif  // PRT_INCOME_H
