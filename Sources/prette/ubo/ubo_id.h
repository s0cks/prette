#ifndef PRT_UBO_ID_H
#define PRT_UBO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt {
  namespace ubo {
    typedef GLuint UboId;
    static constexpr const auto kInvalidUboId = 0;

    static inline bool
    IsInvalidUboId(const UboId id) {
      return id == kInvalidUboId;
    }

    static inline bool
    IsValidUboId(const UboId id) {
      return id >= 0;
    }

    void GenerateUboIds(UboId* ids, const int num_ids);
    rx::observable<UboId> GenerateUboIdsAsync(const int num);

    static inline UboId
    GenerateUboId() {
      UboId id;
      GenerateUboIds(&id, 1);
      return id;
    }

    static inline rx::observable<UboId>
    GenerateUboIdAsync() {
      return GenerateUboIdsAsync(1);
    }
  }
  using ubo::UboId;
  using ubo::kInvalidUboId;
  using ubo::IsValidUboId;
  using ubo::IsInvalidUboId;
}

#endif //PRT_UBO_ID_H