#include "prette/entity/entity_system.h"

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/entity/entity.h"
#include "prette/entity/entity_generation.h"
#include "prette/entity/entity_id.h"
#include "prette/platform.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<EntitySystem> system_{};

EntitySystem::EntitySystem() :
  current_gen_(CreateNewGeneration()) {}

EntitySystem::~EntitySystem() = default;

auto EntitySystem::CreateNewGeneration() -> entity::Generation* {
  const auto next_id = (uword)num_generations_;
  num_generations_ += 1;
  const auto next_gen = new entity::Generation(next_id);
  Append(&generations_, next_gen);
  return next_gen;
}

auto EntitySystem::CreateNewEntity() -> Entity {
  const auto current_gen = GetCurrentGeneration();
  EntityId id = kInvalidEntityId;
  if (!current_gen->GetNextEntityId(&id)) {
    // TODO: allocate a new generation
    return {};
  }
  return Entity(id);
}

void EntitySystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new EntitySystem();
}

auto EntitySystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto EntitySystem::GetSystem() -> EntitySystem* {
  ASSERT(IsSystemInitialized());
  return system_.Get();
}
}  // namespace prt