#ifndef PRT_ENTITY_ID_H
#define PRT_ENTITY_ID_H

#include <cstdint>

namespace prt {
using EntityId = uint64_t;
static constexpr const EntityId kInvalidEntityId = 0;
}  // namespace prt

#endif  // PRT_ENTITY_ID_H
