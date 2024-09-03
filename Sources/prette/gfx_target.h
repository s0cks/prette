#ifndef PRT_GFX_TARGET_H
#define PRT_GFX_TARGET_H

#include "prette/gfx.h"

namespace prt::gfx {
  enum Target : GLenum {
    kArrayBufferTarget = GL_ARRAY_BUFFER,
    kElementArrayBufferTarget = GL_ELEMENT_ARRAY_BUFFER,

    kVboTarget = kArrayBufferTarget,
    kIboTarget = kElementArrayBufferTarget,
  };
}

#endif //PRT_GFX_TARGET_H