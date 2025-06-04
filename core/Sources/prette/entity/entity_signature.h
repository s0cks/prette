#ifndef PRT_ENTITY_SIGNATURE_H
#define PRT_ENTITY_SIGNATURE_H

#include <bitset>

#ifndef PRT_ENTITY_SIGNATURE_SIZE
#define PRT_ENTITY_SIGNATURE_SIZE 32
#endif  // PRT_ENTITY_SIGNATURE_SIZE

namespace prt {
static constexpr const auto kTotalNumberOfComponentsPerEntity = PRT_ENTITY_SIGNATURE_SIZE;
using EntitySignature = std::bitset<kTotalNumberOfComponentsPerEntity>;
}  // namespace prt

#endif  // PRT_ENTITY_SIGNATURE_H
