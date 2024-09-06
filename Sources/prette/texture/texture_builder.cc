#include "prette/texture/texture_builder.h"
#include "prette/texture/texture.h"

namespace prt::texture {
  TextureId TextureBuilderBase::Build() const {
    const auto id = GenerateTextureId();
    PRT_ASSERT(IsValidTextureId(id));
    Texture::BindTexture(GetTarget(), id);
    filter_.ApplyTo(GetTarget());
    wrap_.ApplyTo(GetTarget());
    InitTexture(id);
    Texture::UnbindTexture(GetTarget());
    return id;
  }
  
  rx::observable<TextureId> TextureBuilderBase::BuildAsync() const {
    return rx::observable<>::create<TextureId>([this](rx::subscriber<TextureId> s) {
      const auto id = Build();
      if(IsInvalidTextureId(id)) {
        const auto err = fmt::format("build returned invalid TextureId: {0:d}", id);
        return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
      }
      
      s.on_next(id);
      s.on_completed();
    });
  }

  void TextureBuilder::InitTexture(const TextureId id) const {
    PRT_ASSERT(IsValidTextureId(id));
    if(!HasData())
      return;
    const auto format = GetData()->GetFormat();
    const auto data = GetData()->data();
    const auto width = GetData()->GetWidth();
    const auto height = GetData()->GetHeight(); 
    switch(GetTarget()) {
      case k2D: {
        glTexImage2D(GetTarget(), GetLevel(), (GLint) format, width, height, GetBorder(), (GLint) format, GL_UNSIGNED_BYTE, data); 
        CHECK_GL;
        break;
      }
      default: {
        DLOG(ERROR) << "unknown TextureTarget: " << GetTarget();
        break;
      }
    }
  }
}