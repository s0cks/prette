#ifndef PRT_FBO_ID_H
#define PRT_FBO_ID_H

#include "prette/gfx_object_id.h"

namespace prt {
  namespace fbo {
    typedef gfx::ObjectId<GLuint, &glGenFramebuffers> FboId;

    static constexpr const FboId kDefaultFboId = 0;
  }
  using fbo::FboId;
  using fbo::kDefaultFboId;
}

#endif //PRT_FBO_ID_H