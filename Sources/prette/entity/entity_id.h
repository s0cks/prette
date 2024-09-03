#ifndef PRT_ENTITY_ID_H
#define PRT_ENTITY_ID_H

#include <cstdint>

namespace prt {
  typedef uint64_t EntityId;
  static constexpr const EntityId kInvalidEntityId = 0;
}

#endif //PRT_ENTITY_ID_H