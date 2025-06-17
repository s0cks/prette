#ifndef PRT_FORENAME_GEN_H
#define PRT_FORENAME_GEN_H

#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "prette/citizen/gender.h"

namespace prt {
class ForenameGenerator {
 private:
  std::mt19937 random_{};
  std::vector<std::string> names_{};

 public:
  explicit ForenameGenerator(const Gender gender);
  ForenameGenerator(const uint64_t seed, const Gender gender);
  ~ForenameGenerator() = default;
  auto Generate() -> std::string;

  inline auto operator()() -> std::string {
    return Generate();
  }

  inline operator std::string() {
    return Generate();
  }
};

class SurnameGenerator {
 private:
  std::mt19937 random_{};
  std::vector<std::string> names_{};

 public:
  SurnameGenerator();
  explicit SurnameGenerator(const uint64_t seed);
  ~SurnameGenerator() = default;
  auto Generate() -> std::string;

  inline auto operator()() -> std::string {
    return Generate();
  }

  inline operator std::string() {
    return Generate();
  }
};

class FullnameGenerator {
 private:
  ForenameGenerator forename_;
  SurnameGenerator surname_;

 public:
  explicit FullnameGenerator(const Gender gender) :
    forename_(gender),
    surname_() {}
  FullnameGenerator(const Gender gender, const uint64_t seed) :
    forename_(seed, gender),
    surname_(seed) {}
  ~FullnameGenerator() = default;

  auto Generate() -> std::pair<std::string, std::string> {
    return std::make_pair(forename_(), surname_());
  }

  inline auto operator()() -> std::pair<std::string, std::string> {
    return Generate();
  }

  inline operator std::pair<std::string, std::string>() {
    return Generate();
  }
};
}  // namespace prt

#endif  // PRT_FORENAME_GEN_H
