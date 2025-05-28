#ifndef PRT_CONTENT_TYPE_H
#define PRT_CONTENT_TYPE_H

#include <cstdint>
#include <ostream>

namespace prt::asset {
#define FOR_EACH_CONTENT_TYPE(V)                   \
  V(ApplicationBinary, "application/octet-stream") \
  V(ApplicationJson, "application/json")           \
  V(ApplicationLua, "script/lua")                  \
  V(ImageJpeg, "image/jpeg")                       \
  V(ImagePng, "image/png")                         \
  V(AudioWave, "audio/wav")                        \
  V(FontTruetype, "font/ttf")                      \
  V(TextGlsl, "text/glsl")                         \
  V(TextGlslVertexShader, "text/glsl+vertex")      \
  V(TextGlslFragmentShader, "text/glsl+fragment")  \
  V(SpirvVertexShader, "application/spirv+vertex") \
  V(SpirvFragmentShader, "application/spirv-fragment")

enum ContentType : uint8_t {
  kInvalidContentType = 0,
#define DEFINE_CONTENT_TYPE(Name, Value) k##Name,
  FOR_EACH_CONTENT_TYPE(DEFINE_CONTENT_TYPE)
#undef DEFINE_CONTENT_TYPE
  // clang-format off
  kTotalNumberOfContentTypes,
  kDefaultContentType = kApplicationBinary,
  // clang-format on
};

static inline auto operator<<(std::ostream& stream, const ContentType& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name, Value) \
  case ContentType::k##Name:         \
    return stream << (Value);
    FOR_EACH_CONTENT_TYPE(DEFINE_TOSTRING);
#undef DEFINE_TOSTRING
    case ContentType::kInvalidContentType:
    default:
      return stream << "Invalid ContentType: " << static_cast<uint64_t>(rhs);
  }
}
}  // namespace prt::asset

#endif  // PRT_CONTENT_TYPE_H
