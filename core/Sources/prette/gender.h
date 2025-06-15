#ifndef PRT_GENDER_H
#define PRT_GENDER_H

#include <cstdint>
#include <ostream>

namespace prt {
enum Gender : uint8_t {
  kMale,
  kFemale,
};

static inline auto operator<<(std::ostream& stream, const Gender& rhs) -> std::ostream& {
  switch (rhs) {
    case Gender::kMale:
      return stream << "male";
    case Gender::kFemale:
      return stream << "female";
    default:
      return stream << "undefined";
  }
}
}  // namespace prt

#endif  // PRT_GENDER_H
