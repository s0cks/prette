#include "prette/fbo/fbo_attachment.h"

#include <sstream>

#include "prette/fbo/fbo.h"
#include "prette/render/render_settings.h"
#include "prette/texture/texture_builder.h"

namespace prt::fbo {
  TextureId ColorAttachment::GetTextureId() const {
    return GetTexture()->GetId();
  }

  std::string ColorAttachment::ToString() const {
    std::stringstream ss;
    ss << "fbo::ColorAttachment(";
    ss << "texture=" << GetTexture()->ToString();
    ss << ")";
    return ss.str();
  }

  void ColorAttachment::AttachTo(const FboTarget target) {
    Fbo::AttachTexture2D(target, GetAttachmentPoint(), GetTexture()->GetId());
  }

  ColorAttachment* ColorAttachment::New(const texture::TextureFormat format,
                                        const texture::TextureSize& size,
                                        const Level level) {
    using namespace texture;
    TextureBuilder builder(k2D);
    builder.SetFormat(format);
    builder.SetInternalFormat(format);
    builder.SetSize(size);
    builder.SetFilter(texture::kLinearFilter);
    builder.SetWrap(texture::kClampToEdgeWrap);
    builder.SetType(GL_UNSIGNED_BYTE);
    const auto texture = Texture2d::New(builder.Build());
    return new ColorAttachment(texture, size, level);
  }

  ColorAttachment* ColorAttachment::NewDefaultResolution(const Level level) {
    const auto resolution = render::GetResolution();
    return New(texture::kRGBA, texture::TextureSize(resolution.width(), resolution.height()), level);
  }
}