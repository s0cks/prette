#ifndef PRT_VBO_ID_H
#define PRT_VBO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt {
  namespace vbo {
    typedef GLuint VboId;
    static constexpr const VboId kInvalidVboId = -1;
    static constexpr const VboId kDefaultVboId = 0;

    static inline bool
    IsDefaultVboId(const VboId id) {
      return id == kDefaultVboId;
    }

    static inline bool
    IsInvalidVboId(const VboId id) {
      return id == kInvalidVboId;
    }

    static inline bool
    IsValidVboId(const VboId id) {
      return id >= kDefaultVboId;
    }

    void GenerateVboIds(VboId* ids, const int num);

    static inline VboId
    GenerateVboId() {
      VboId id;
      GenerateVboIds(&id, 1);
      return id;
    }

    static inline rx::observable<VboId>
    GenerateVboIdsAsync(const int num) {
      return rx::observable<>::create<VboId>([num](rx::subscriber<VboId> s) {
        VboId ids[num];
        GenerateVboIds(&ids[0], num);
        for(auto idx = 0; idx < num; idx++)
          s.on_next(ids[idx]);
        s.on_completed();
      });
    }
  }
  using vbo::VboId;
  using vbo::kInvalidVboId;
}

#endif //PRT_VBO_ID_H