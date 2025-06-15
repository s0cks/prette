#include "prette/person_gen.h"

#include <cstdint>

#include "prette/assertions.h"
#include "prette/gender.h"
#include "prette/person.h"

namespace prt {
PersonGenerator::PersonGenerator(const uint64_t seed) :
  gen_(seed),
  male_name_gen_(Gender::kMale, seed),
  female_name_gen_(Gender::kFemale, seed) {}

PersonGenerator::~PersonGenerator() = default;

auto PersonGenerator::GenerateWithGender(Gender gender, Person* result) -> bool {
  PersonBuilder builder{};
  const auto [forename, surname] = GenerateFullname(gender);
  // clang-format off
  (*result) = builder.WithGender(gender)
    .WithFirstName(forename)
    .WithLastName(surname);
  // clang-format on
  return true;
}

auto PersonGenerator::Generate(Person* result) -> bool {
  ASSERT(result);
  return GenerateWithGender(PickGender(), result);
}
}  // namespace prt