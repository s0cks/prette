#ifndef PRT_ENTITY_GENERATION_H
#define PRT_ENTITY_GENERATION_H

#include <array>
#include <gflags/gflags.h>

#include "prette/assertions.h"
#include "prette/entity/entity_id.h"
#include "prette/entity/entity_signature.h"
#include "prette/platform.h"
#include "prette/relaxed_atomic.h"

#ifndef PRT_MAX_GENERATION_SIZE
#define PRT_MAX_GENERATION_SIZE 65536
#endif  // PRT_MAX_GENERATION_SIZE

namespace prt {
class EntitySystem;
}

namespace prt::entity {
using GenerationId = uword;

static constexpr const uword kMaxGenerationSize = PRT_MAX_GENERATION_SIZE;
using GenerationSignatureData = std::array<EntitySignature, kMaxGenerationSize>;

class Generation {
  friend class prt::EntitySystem;

 private:
  Generation* next_ = nullptr;
  GenerationId id_;
  RelaxedAtomic<EntityId> num_entities_ = 0;
  GenerationSignatureData signatures_{};

  auto GetNextEntityId(EntityId* id) -> bool {
    if ((GetNumberOfEntities() + 1) > kMaxGenerationSize) {
      (*id) = kInvalidEntityId;
      return false;
    }
    num_entities_ += 1;
    (*id) = (EntityId)num_entities_;
    return true;
  }

 public:
  explicit Generation(GenerationId id) :
    id_(id) {}
  ~Generation() = default;

  auto GetId() const -> GenerationId {
    return id_;
  }

  auto GetNumberOfEntities() const -> uword {
    return static_cast<uword>((EntityId)num_entities_);
  }

  auto GetNext() const -> Generation* {
    return next_;
  }

  inline auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }

  void SetNext(Generation* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }

  auto GetSignatureData() const -> const GenerationSignatureData& {
    return signatures_;
  }

  auto GetSignatureAt(const uint64_t idx) const -> const EntitySignature& {
    return signatures_.at(idx);
  }

  void SetSignatureAt(const uint64_t idx, const EntitySignature& rhs) {
    signatures_.at(idx) = rhs;
  }
};
}  // namespace prt::entity

#endif  // PRT_ENTITY_GENERATION_H
