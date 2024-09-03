#include "prette/vao/vao_id.h"

namespace prt::vao {
  void GenerateVaoIds(VaoId* ids, const int num_ids) {
    PRT_ASSERT(num_ids >= 1);
    glGenVertexArrays(num_ids, ids);
    CHECK_GL(FATAL);
  }

  rx::observable<VaoId> GenerateVaoIdsAsync(const int num_ids) {
    PRT_ASSERT(num_ids >= 1);
    return rx::observable<>::create<VaoId>([num_ids](rx::subscriber<VaoId> s) {
      VaoId ids[num_ids];
      GenerateVaoIds(&ids[0], num_ids);

      for(auto idx = 0; idx < num_ids; idx++)
        s.on_next(ids[idx]);
      s.on_completed();
    });
  }
}