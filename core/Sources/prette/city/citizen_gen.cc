#include "prette/city/citizen_gen.h"

#include <cstdint>

#include "prette/assertions.h"
#include "prette/city/citizen.h"

namespace prt {
CitizenGenerator::CitizenGenerator(const uint64_t seed) :
  gen_(seed),
  male_name_gen_(Gender::kMale, seed),
  female_name_gen_(Gender::kFemale, seed) {}

CitizenGenerator::~CitizenGenerator() = default;

auto CitizenGenerator::GenerateWithGender(Gender gender, Citizen* result) -> bool {
  CitizenBuilder builder{};
  const auto [forename, surname] = GenerateFullname(gender);
  // clang-format off
  (*result) = builder.WithGender(gender)
    .WithForename(forename)
    .WithSurname(surname);
  // clang-format on
  return true;
}

auto CitizenGenerator::Generate(Citizen* result) -> bool {
  ASSERT(result);
  return GenerateWithGender(PickGender(), result);
}
}  // namespace prt