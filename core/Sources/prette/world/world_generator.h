#ifndef PRT_WORLD_GENERATOR_H
#define PRT_WORLD_GENERATOR_H

#include <exception>
#include <gflags/gflags.h>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/world/world.h"

namespace prt {
DECLARE_string(world_name);

static inline auto GetWorldName() -> std::string {
  return FLAGS_world_name;
}

class WorldGenerationException : public std::exception {
 private:
  std::string message_{};

 public:
  WorldGenerationException(const std::string message) :
    std::exception(),
    message_(std::move(message)) {}
  ~WorldGenerationException() override = default;

  auto what() const noexcept -> const char* override {
    return message_.c_str();
  }
};

class WorldInitializer {
 private:
  WorldInitializer* next_ = nullptr;

 protected:
  WorldInitializer() = default;

 public:
  virtual ~WorldInitializer() = default;
  virtual auto GetName() const -> const char* = 0;
  virtual auto Apply(World* world) -> bool = 0;

  auto GetNext() const -> WorldInitializer* {
    return next_;
  }

  inline auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }

  void SetNext(WorldInitializer* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }
};

class WorldInitializerIterator {
 private:
  WorldInitializer* current_;

 public:
  explicit WorldInitializerIterator(WorldInitializer* head) :
    current_(head) {}
  ~WorldInitializerIterator() = default;

  auto HasNext() const -> bool {
    return current_ != nullptr;
  }

  auto Next() -> WorldInitializer* {
    const auto next = current_;
    current_ = next->GetNext();
    return next;
  }
};

class WorldGenerator {
 private:
  WorldInitializer* initializers_ = nullptr;

 protected:
  WorldGenerator() = default;

  auto GetInitializerList() const -> WorldInitializer* {
    return initializers_;
  }

  void AddInitializer(WorldInitializer* rhs) {
    ASSERT(rhs);
    Append(&initializers_, rhs);
  }

  void RemoveInitializer(WorldInitializer* rhs) {
    ASSERT(rhs);
    Remove(&initializers_, rhs);
  }

 public:
  virtual ~WorldGenerator() = default;
  virtual auto GetName() const -> const char* = 0;
  virtual auto Generate() -> World* = 0;
};

class DefaultWorldGenerator : public WorldGenerator {
 public:
  DefaultWorldGenerator();
  ~DefaultWorldGenerator() override;

  auto GetName() const -> const char* override {
    return "Default";
  }

  auto Generate() -> World* override;
};
}  // namespace prt

#endif  // PRT_WORLD_GENERATOR_H
