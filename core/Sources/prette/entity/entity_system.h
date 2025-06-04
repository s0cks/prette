#ifndef PRT_ENTITY_SYSTEM_H
#define PRT_ENTITY_SYSTEM_H

#include "prette/entity/entity.h"
#include "prette/entity/entity_generation.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"

namespace prt {
class EntitySystem {
  friend class Entity;

 public:
  static constexpr const auto kSystemName = "Entity";

 private:
  RelaxedAtomic<uword> num_generations_ = 0;
  entity::Generation* generations_ = nullptr;
  entity::Generation* current_gen_ = nullptr;

  auto CreateNewGeneration() -> entity::Generation*;

 public:
  EntitySystem();
  ~EntitySystem();

  auto GetNumberOfGenerations() const -> uword {
    return (uword)num_generations_;
  }

  auto GetGenerationsList() const -> entity::Generation* {
    return generations_;
  }

  auto GetCurrentGeneration() const -> entity::Generation* {
    return current_gen_;
  }

  auto CreateNewEntity() -> Entity;

 public:
  static void InitSystem();
  static auto IsSystemInitialized() -> bool;
  static auto GetSystem() -> EntitySystem*;
};
}  // namespace prt

#endif  // PRT_ENTITY_SYSTEM_H
