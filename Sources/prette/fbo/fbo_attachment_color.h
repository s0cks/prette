#ifndef PRT_FBO_ATTACHMENT_H
#error "Please #include <prette/fbo/fbo_attachment.h> instead."
#endif //PRT_FBO_ATTACHMENT_H

#ifndef PRT_FBO_ATTACHMENT_COLOR_H
#define PRT_FBO_ATTACHMENT_COLOR_H

#include "prette/texture/texture.h"
#include "prette/fbo/fbo_attachment.h"
#include "prette/texture/texture_format.h"

namespace prt::fbo {
  using texture::Texture;
  class ColorAttachment : public Attachment {
  public:
    enum Level : GLenum {
#define DEFINE_LEVEL(Name, GlValue) k##Name = (GlValue),
      FOR_EACH_FBO_COLOR_ATTACHMENT_POINT(DEFINE_LEVEL)
#undef DEFINE_LEVEL
      kMaxLevels = GL_MAX_COLOR_ATTACHMENTS,
    };
  protected:
    Texture* texture_;
    texture::TextureSize size_;
    Level level_;

    explicit ColorAttachment(Texture* texture, const texture::TextureSize& size, const Level level):
      Attachment(),
      texture_(texture),
      size_(size),
      level_(level) {
    }

    void AttachTo(const FboTarget target = kDefaultTarget) override;
  public:
    ~ColorAttachment() override = default;

    virtual Texture* GetTexture() const {
      return texture_;
    }

    virtual TextureId GetTextureId() const;

    virtual Level GetLevel() const {
      return level_;
    }

    const texture::TextureSize& GetSize() const {
      return size_;
    }

    AttachmentPoint GetAttachmentPoint() const override {
      return static_cast<AttachmentPoint>(GetLevel());
    }

    DECLARE_FBO_ATTACHMENT(Color);
  public:
    static ColorAttachment* New(const texture::TextureFormat format,
                                const texture::TextureSize& size,
                                const Level level = k0);
    static ColorAttachment* NewDefaultResolution(const Level level = k0);
  };
}

#endif //PRT_FBO_ATTACHMENT_COLOR_H