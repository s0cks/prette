#ifndef PRT_FBO_ID_H
#define PRT_FBO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"

namespace prt {
  namespace fbo {
    typedef GLuint FboId;
    static constexpr const FboId kInvalidFboId = -1;
    static constexpr const FboId kDefaultFboId = 0;

    static inline bool IsValidFboId(const FboId id) {
      return id >= kDefaultFboId;
    }

    static inline bool IsDefaultFboId(const FboId id) {
      return id == kDefaultFboId;
    }

    void GenerateFboIds(FboId* ids, const int num);

    static inline FboId
    GenerateFboId() {
      FboId id;
      GenerateFboIds(&id, 1);
      return id;
    }

    static inline rx::observable<FboId>
    GenerateFboIdsAsync(const int num) {
      return rx::observable<>::create<FboId>([num](rx::subscriber<FboId> s) {
        FboId ids[num];
        GenerateFboIds(&ids[0], num);
        for(auto idx = 0; idx < num; idx++)
          s.on_next(ids[idx]);
        s.on_completed();
      });
    }
  }
  using fbo::FboId;
  using fbo::kInvalidFboId;
  using fbo::kDefaultFboId;
}

#endif //PRT_FBO_ID_H