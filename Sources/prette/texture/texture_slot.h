#ifndef PRT_TEXTURE_SLOT_H
#define PRT_TEXTURE_SLOT_H

#include "prette/gfx.h"

namespace prt::texture {
  typedef GLenum TextureSlot;

  static constexpr const TextureSlot kTextureSlotMax = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
}

#endif //PRT_TEXTURE_SLOT_H