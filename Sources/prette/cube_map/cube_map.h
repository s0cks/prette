#ifndef PRT_TEXTURE_H
#error "Please #include <prette/texture/texture.h> instead."
#endif //PRT_TEXTURE_H

#ifndef PRT_CUBE_MAP_H
#define PRT_CUBE_MAP_H

#include "prette/texture/texture.h"
#include "prette/cube_map/cube_map_face.h"
#include "prette/cube_map/cube_map_builder.h"

namespace prt {
  namespace texture {
    class CubeMap : public TextureTemplate<kCubeMap> {
    protected:
      explicit CubeMap(const TextureId id);
    public:
      ~CubeMap() override;
      std::string ToString() const override;
    private:
      static inline CubeMap*
      New(const TextureId id) {
        return new CubeMap(id);
      }
    public:
      static inline CubeMap*
      New(const CubeMapBuilder& builder) {
        return New(builder.Build());
      }

      static CubeMap* New(const uri::Uri& uri);

      static inline CubeMap*
      New(const uri::basic_uri& uri) {
        return New(uri::Uri(uri));
      }
    };
  }
  using texture::CubeMap;
}

#endif //PRT_CUBE_MAP_H