#ifndef PRT_VAO_ID_H
#define PRT_VAO_ID_H

#include "prette/rx.h"
#include "prette/gfx.h"
#include "prette/gfx_object_id.h"

namespace prt {
  namespace vao {
    typedef gfx::ObjectId<GLuint, &glGenVertexArrays> VaoId;

    static constexpr const VaoId kDefaultVaoId = 0;
    static constexpr const VaoId kInvalidVaoId = -1;
  }
  using vao::VaoId;
  using vao::kDefaultVaoId;
  using vao::kInvalidVaoId;
}

#endif //PRT_VAO_ID_H