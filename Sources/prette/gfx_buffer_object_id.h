#ifndef PRT_GFX_BUFFER_ID_H
#define PRT_GFX_BUFFER_ID_H

#include "prette/gfx.h"

namespace prt {
  namespace gfx {
    typedef GLuint BufferObjectId;
    static constexpr const BufferObjectId kInvalidBufferObjectId = 0;

    static inline bool
    IsValidBufferObjectId(const BufferObjectId id) {
      return id >= 0;
    }

    static inline bool
    IsInvalidBufferObjectId(const BufferObjectId id) {
      return id == kInvalidBufferObjectId;
    }
  }
  using gfx::BufferObjectId;
  using gfx::kInvalidBufferObjectId;
  using gfx::IsValidBufferObjectId;
  using gfx::IsInvalidBufferObjectId;
}

#endif //PRT_GFX_BUFFER_ID_H