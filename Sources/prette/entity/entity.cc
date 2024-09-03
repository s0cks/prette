#include "prette/entity/entity.h"

#include "prette/entity/generation.h"

namespace prt {
  namespace entity {
    static rx::subject<EntityEvent*> events_;

    rx::observable<EntityEvent*> Entity::OnEvent() const {
      return entity::OnEvent(id());
    }

#define DEFINE_ON_EVENT(Name)                                     \
    rx::observable<Name##Event*> Entity::On##Name() const {       \
      return entity::On##Name##Event(id());                       \
    }
    FOR_EACH_ENTITY_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT

    rx::observable<EntityEvent*> OnEvent() {
      return events_.get_observable();
    }
  }
}