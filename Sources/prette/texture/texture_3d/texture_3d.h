#ifndef PRT_TEXTURE_H
#error "Please #include <prette/texture/texture.h> instead."
#endif //PRT_TEXTURE_H

#ifndef PRT_TEXTURE_3D_H
#define PRT_TEXTURE_3D_H

#include "prette/texture/texture.h"

namespace prt {
  namespace texture {
    class Texture3d : public TextureTemplate<k3D> {
    protected:
      explicit Texture3d(const TextureId id):
        TextureTemplate<k3D>(id) {
      }
    public:
      ~Texture3d() override = default;
      std::string ToString() const override;
    public:
      static Texture3d* New(const json::TextureValue* value);
    };
  }
  using texture::Texture3d;
}

#endif //PRT_TEXTURE_3D_H