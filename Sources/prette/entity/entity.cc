#include "prette/entity/entity.h"
#include "prette/entity/entity_events.h"
#include "prette/entity/generation.h"

namespace prt::entity {
  static rx::subject<EntityEvent*> events_;

  auto Entity::OnEvent() const -> rx::observable<EntityEvent*> {
    return entity::OnEvent(id());
  }

#define DEFINE_ON_EVENT(Name)                                         \
  auto Entity::On##Name() const -> rx::observable<Name##Event*> {   \
    return entity::On##Name##Event(id());                           \
  }
  FOR_EACH_ENTITY_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

  auto OnEvent() -> rx::observable<EntityEvent*> {
    return events_.get_observable();
  }
}