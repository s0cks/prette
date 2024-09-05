#include "prette/ubo/ubo_id.h"

namespace prt::ubo {
  void GenerateUboIds(UboId* ids, const int num_ids) {
    glGenBuffers(num_ids, &ids[0]);
    CHECK_GL;
  }

  rx::observable<UboId> GenerateUboIdsAsync(const int num_ids) {
    return rx::observable<>::create<UboId>([num_ids](rx::subscriber<UboId> s) {
      UboId ids[num_ids];
      GenerateUboIds(&ids[0], num_ids);
      for(auto idx = 0; idx < num_ids; idx++)
        s.on_next(ids[idx]);
      s.on_completed();
    });
  }
}