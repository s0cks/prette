#include "prette/name_gen.h"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

#include "prette/city/gender.h"
#include "prette/common.h"
#include "prette/names.h"

namespace prt {
static inline void LoadListDB(const std::string value, std::vector<std::string>& results) {
  return Split(value, '\n', results);
}

static inline void LoadListDB(const uint8_t* bytes, const uint64_t num_bytes, std::vector<std::string>& results) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  return LoadListDB(std::string((const char*)bytes, num_bytes), results);
}

static inline void LoadForenamesDB(const Gender gender, std::vector<std::string>& results) {
  switch (gender) {
    case kMale:
      return LoadListDB(forenames::male::kBytes, forenames::male::kTotalNumberOfBytes, results);
    case kFemale:
      return LoadListDB(forenames::female::kBytes, forenames::female::kTotalNumberOfBytes, results);
    default:
      LOG(FATAL) << "invalid gender for forenames: " << gender;
  }
}

static inline void LoadSurnamesDB(std::vector<std::string>& results) {
  return LoadListDB(surnames::kBytes, surnames::kTotalNumberOfBytes, results);
}

ForenameGenerator::ForenameGenerator(const Gender gender) :
  random_(std::random_device{}()),
  names_() {
  LoadForenamesDB(gender, names_);
}

ForenameGenerator::ForenameGenerator(const uint64_t seed, const Gender gender) :
  random_(seed),
  names_() {
  LoadForenamesDB(gender, names_);
}

auto ForenameGenerator::Generate() -> std::string {
  std::uniform_int_distribution<uint64_t> distribution(0, names_.size() - 1);
  return names_.at(distribution(random_));
}

SurnameGenerator::SurnameGenerator() :
  random_(std::random_device{}()),
  names_() {
  LoadSurnamesDB(names_);
}

SurnameGenerator::SurnameGenerator(const uint64_t seed) :
  random_(seed),
  names_() {
  LoadSurnamesDB(names_);
}

auto SurnameGenerator::Generate() -> std::string {
  std::uniform_int_distribution<uint64_t> distribution(0, names_.size() - 1);
  return names_.at(distribution(random_));
}
}  // namespace prt