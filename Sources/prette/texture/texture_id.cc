#include "prette/texture/texture_id.h"

namespace prt::texture {
  void GenerateTextureIds(TextureId* ids, const int num) {
    glGenTextures(num, &ids[0]);
    CHECK_GL;
  }

  rx::observable<TextureId> GenerateTextureIdsAsync(const int num) {
    return rx::observable<>::create<TextureId>([num](rx::subscriber<TextureId> s) {
      TextureId ids[num];
      GenerateTextureIds(&ids[0], num);
      for(auto idx = 0; idx < num; idx++)
        s.on_next(ids[idx]);
      s.on_completed();
    });
  }
}