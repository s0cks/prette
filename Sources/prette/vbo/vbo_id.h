#ifndef PRT_VBO_ID_H
#define PRT_VBO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"
#include "prette/gfx_object_id.h"

namespace prt {
  namespace vbo {
    typedef gfx::ObjectId<GLuint, &glGenBuffers> VboId;

    static constexpr const VboId kDefaultVboId = 0;
    static constexpr const VboId kInvalidVboId = -1;
  }
  using vbo::VboId;
  using vbo::kInvalidVboId;
}

#endif //PRT_VBO_ID_H