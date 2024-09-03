#ifndef PRT_TEXTURE_PROPERTY_H
#define PRT_TEXTURE_PROPERTY_H

#include "prette/texture/texture_target.h"
#include "prette/texture/texture_constants.h"

namespace prt::texture {
  class TextureProperty {
  protected:
    virtual void ApplyTo(const TextureTarget id) = 0;
  public:
    TextureProperty() = default;
    virtual ~TextureProperty() = default;
    virtual const char* GetName() const = 0;
  };
}

#endif //PRT_TEXTURE_PROPERTY_H