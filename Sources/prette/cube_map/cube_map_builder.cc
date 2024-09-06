#include "prette/cube_map/cube_map_builder.h"
#include "prette/texture/texture.h"

namespace prt::texture {
  void CubeMapBuilder::SetFace(const CubeMapFace face, const img::Image* image, const int level, const int border) {
    PRT_ASSERT(image);
    switch(image->GetFormat().value()) {
      case GL_RGB: {
        const auto data = FaceData {
          .face = face,
          .border = border,
          .level = level,
          .bytes = (const uint8_t*) image->data(),
          .num_bytes = image->GetTotalSize(),
          .internal_format = kRGB,
          .format = kRGB,
          .type = GL_UNSIGNED_BYTE,
          .size = glm::vec2(image->GetWidth(), image->GetHeight()),
        };
        const auto result = faces_.insert(data);
        LOG_IF(ERROR, !result.second) << "failed to insert: " << data;
        break;
      }
      case GL_RGBA: {
        const auto data = FaceData {
          .face = face,
          .border = border,
          .level = level,
          .bytes = (const uint8_t*) image->data(),
          .num_bytes = image->GetTotalSize(),
          .internal_format = kRGBA,
          .format = kRGBA,
          .type = GL_UNSIGNED_BYTE,
          .size = glm::vec2(image->GetWidth(), image->GetHeight()),
        };
        const auto result = faces_.insert(data);
        LOG_IF(ERROR, !result.second) << "failed to insert: " << data;
        break;
      }
      default:
        LOG(ERROR) << "invalid image type: " << image->GetFormat();
        break;
    }
  }

  void CubeMapBuilder::InitTexture(const TextureId id) const {
    PRT_ASSERT(IsValidTextureId(id));
    for(const auto& face : faces_) {
      glTexImage2D(
        face.face,
        face.level,
        (GLenum) face.internal_format,
        face.size[0],
        face.size[1],
        face.border,
        (GLenum) face.format,
        face.type,
        face.bytes
      );
    }
  }
}