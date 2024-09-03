#ifndef PRT_RENDERBUFFER_H
#define PRT_RENDERBUFFER_H

#include "prette/gfx.h"

namespace prt {
  typedef GLuint RenderBufferObjectId;
  static constexpr const RenderBufferObjectId kInvalidRenderBufferId = -1;

#define FOR_EACH_DEPTH_BUFFER_FORMAT(V) \
  V(Depth, GL_DEPTH_COMPONENT)          \
  V(Depth16, GL_DEPTH_COMPONENT16)      \
  V(Depth32, GL_DEPTH_COMPONENT32)      \
  V(Depth32f, GL_DEPTH_COMPONENT32F)

#define FOR_EACH_RENDER_BUFFER_FORMAT(V) \
  FOR_EACH_DEPTH_BUFFER_FORMAT(V)

  enum RenderBufferFormat {
#define DEFINE_RENDER_BUFFER_FORMAT(Name, Value) \
    k##Name##Format = Value,
    FOR_EACH_RENDER_BUFFER_FORMAT(DEFINE_RENDER_BUFFER_FORMAT)
#undef DEFINE_RENDER_BUFFER_FORMAT
  };

  enum RenderBufferTarget {
    kRenderBuffer = GL_RENDERBUFFER,

    kDefaultRenderBufferTarget = kRenderBuffer,
  };

  template<const RenderBufferFormat Format,
           const RenderBufferTarget Target = kDefaultRenderBufferTarget>
  class RenderBuffer : public gfx::Resource {
  private:
    RenderBufferObjectId id_;
    Dimension size_;

    inline void Initialize(const bool generate,
                           const bool bind,
                           const bool unbind) {
      if(!generate)
        return;
      glGenRenderbuffers(1, &id_);
      CHECK_GL(FATAL);
      if(!bind)
        return;
      glBindRenderbuffer(Target, id_);
      CHECK_GL(FATAL);
      glRenderbufferStorage(Target, Format, size_[0], size_[1]);
      CHECK_GL(FATAL);
      if(!unbind)
        return;
      glBindRenderbuffer(Target, 0);
      CHECK_GL(FATAL);
    }
  public:
    RenderBuffer(const Dimension& size,
                 const bool generate = true,
                 const bool bind = true,
                 const bool unbind = true):
      Resource(),
      id_(kInvalidRenderBufferId),
      size_(size) {
      Initialize(generate, bind, unbind);
    }
    ~RenderBuffer() override = default;

    RenderBufferObjectId id() const {
      return id_;
    }

    Dimension size() const {
      return size_;
    }

    uint64_t width() const {
      return size_[0];
    }

    uint64_t height() const {
      return size_[1];
    }

    RenderBufferFormat format() const {
      return Format;
    }

    RenderBufferTarget target() const {
      return Target;
    }

    void Bind() const override {
      glBindRenderbuffer(Target, id_);
      CHECK_GL(FATAL);
    }

    void Unbind() const override {
      glBindRenderbuffer(Target, 0);
      CHECK_GL(FATAL);
    }

    void Delete() {
      glDeleteRenderbuffers(1, &id_);
      CHECK_GL(FATAL);
    }
  };

#define DEFINE_DEPTH_BUFFER(Name, _)                                      \
  class Name##Buffer : public RenderBuffer<k##Name##Format> {             \
  public:                                                                 \
    explicit Name##Buffer(const Dimension& size,                          \
                         const bool generate = true,                      \
                         const bool bind = true,                          \
                         const bool unbind = true):                       \
      RenderBuffer<k##Name##Format>(size, generate, bind, unbind) {       \
    }                                                                     \
    ~Name##Buffer() override = default;                                   \
  };
  FOR_EACH_DEPTH_BUFFER_FORMAT(DEFINE_DEPTH_BUFFER)
#undef DEFINE_DEPTH_BUFFER
}

#endif //PRT_RENDERBUFFER_H