#ifndef PRT_TEXTURE_SCOPE_H
#define PRT_TEXTURE_SCOPE_H

#include "prette/texture/texture.h"
#include "prette/texture/texture_slot.h"

namespace prt::texture {
  class TextureScope {
  protected:
    Texture* texture_;

    explicit TextureScope(Texture* texture):
      texture_(texture) {
    }
  public:
    virtual ~TextureScope() = default;

    Texture* GetTexture() const {
      return texture_;
    }

    TextureId GetTextureId() const {
      return GetTexture()->GetId();
    }
    
    TextureTarget GetTextureTarget() const {
      return GetTexture()->GetTextureTarget();
    }
  };

  template<const TextureSlot Slot>
  class TextureBindScope : public TextureScope {
  public:
    explicit TextureBindScope(Texture* texture):
      TextureScope(texture) {
      Bind(Slot);
    }
    ~TextureBindScope() override {
      Unbind();
    }

    void Bind() const {
      Texture::BindTexture(GetTextureTarget(), GetTextureId());
    }
    
    void Bind(const int32_t slot) const {
      Texture::ActiveTexture(slot);
      Bind();
    }

    void Unbind() const {
      Texture::UnbindTexture(GetTextureTarget());
    }
  };
}

#endif //PRT_TEXTURE_SCOPE_H