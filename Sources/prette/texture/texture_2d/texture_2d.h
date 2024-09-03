#ifndef PRT_TEXTURE_H
#error "Please #include <prette/texture/texture.h> instead."
#endif //PRT_TEXTURE_H

#ifndef PRT_TEXTURE_2D_H
#define PRT_TEXTURE_2D_H

#include "prette/texture/texture.h"
#include "prette/texture/texture_builder.h"

namespace prt {
  namespace texture {
    class Texture2d : public TextureTemplate<k2D> {
    public:
      explicit Texture2d(const TextureId id):
        TextureTemplate<k2D>(id) {
      }
      ~Texture2d() override = default;
      std::string ToString() const override;
    public:
      static inline Texture2d*
      New(const TextureId id) { //TODO: reduce visibility?
        return new Texture2d(id);
      }

      static Texture2d* New(const json::TextureValue* value);
      static Texture2d* New(const uri::Uri& uri);
      
      static inline Texture2d*
      New(const uri::basic_uri& uri) {
        auto target = uri;
        if(!StartsWith(target, "texture"))
          target = fmt::format("texture://{0:s}", uri);
        return New(uri::Uri(target));
      }
    };
  }
  using texture::Texture2d;
}

#endif //PRT_TEXTURE_2D_H