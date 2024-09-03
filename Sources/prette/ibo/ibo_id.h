#ifndef PRT_IBO_ID_H
#define PRT_IBO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt {
  namespace ibo {
    typedef GLuint IboId;
    static constexpr const IboId kInvalidIboId = 0;

    static inline bool
    IsInvalidIboId(const IboId id) {
      return id == kInvalidIboId;
    }

    static inline bool
    IsValidIboId(const IboId id) {
      return id != kInvalidIboId;
    }

    void GenerateIboIds(IboId* ids, const int num_ids);

    static inline IboId
    GenerateIboId() {
      IboId id;
      GenerateIboIds(&id, 1);
      return id;
    }

    rx::observable<IboId> GenerateIboIdsAsync(const int num_ids);

    static inline rx::observable<IboId>
    GenerateIboIdAsync() {
      return GenerateIboIdsAsync(1);
    }
  }
  using ibo::IboId;
}

#endif //PRT_IBO_ID_H