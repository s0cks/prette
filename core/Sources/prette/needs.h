#ifndef PRT_NEEDS_H
#define PRT_NEEDS_H

#include <cstdint>
#include <ostream>

namespace prt {
#define FOR_EACH_NEED(V) \
  V(Water)               \
  V(Power)               \
  V(Food)

enum Needs : uint32_t {
// clang-format off
#define DEFINE_NEED(Name) k##Name,
  FOR_EACH_NEED(DEFINE_NEED)
#undef DEFINE_NEED
  // clang-format on
  kTotalNumberOfNeeds,
};

static inline auto operator<<(std::ostream& stream, const Needs& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name) \
  case Needs::k##Name:        \
    return stream << #Name;
    FOR_EACH_NEED(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return stream << "unknown need";
  }
}
}  // namespace prt

#endif  // PRT_NEEDS_H
