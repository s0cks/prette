#ifndef PRT_CITIZEN_GEN_H
#define PRT_CITIZEN_GEN_H

#include <cstdint>
#include <glog/logging.h>
#include <random>
#include <string>
#include <utility>

#include "prette/city/gender.h"
#include "prette/name_gen.h"

namespace prt {
class Citizen;
class CitizenGenerator {
 private:
  std::mt19937 gen_;
  FullnameGenerator female_name_gen_;
  FullnameGenerator male_name_gen_;

  inline auto PickGender() -> Gender {
    std::uniform_int_distribution<int> distribution(0, 1);
    return static_cast<Gender>(distribution(gen_));
  }

  inline auto GenerateFullname(const Gender rhs) -> std::pair<std::string, std::string> {
    switch (rhs) {
      case kMale:
        return male_name_gen_();
      case kFemale:
        return female_name_gen_();
      default:
        DLOG(FATAL) << "failed to generate fullname, invalid gender: " << rhs;
    }
  }

 public:
  explicit CitizenGenerator(const uint64_t seed);
  ~CitizenGenerator();
  auto GenerateWithGender(Gender gender, Citizen* citizen) -> bool;
  auto Generate(Citizen* result) -> bool;
};
}  // namespace prt

#endif // PRT_CITIZEN_GEN_H
